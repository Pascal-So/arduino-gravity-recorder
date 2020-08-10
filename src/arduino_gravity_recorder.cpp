#include <Arduino.h>
#include <Wire.h>

#include <I2Cdev.h>
#include <MPU9250.h>

#include "cyclic_buffer.hpp"
#include "logger.hpp"

constexpr long serial_baud = 9600;
constexpr uint8_t chipselect_pin = 10;
constexpr uint8_t photo_pin = 2;
constexpr uint32_t clock_frequency_I2C = 400000; // 400kHz

constexpr unsigned int gravity_record_nr = 26;
constexpr unsigned long gravity_record_delay = 10; // ms

MPU9250 accelgyro;

struct gravity_state_t {
	CyclicBuffer<gravity_vector_t, gravity_record_nr> buffer;
	unsigned long next_record_time = 0;
} gravity_state;

struct photo_state_t {
	unsigned long time = 0;
	bool pending = false;
} volatile photo_state;

void photo_interrupt() {
	photo_state.time = millis();
	photo_state.pending = true;
}

SDLib::File logfile;

template <typename T>
inline void serial_println(const T &t) {
#ifndef NDEBUG
	Serial.println(t);
#else
	(void)t;
#endif // NDEBUG
}

template <typename T>
inline void serial_print(const T &t) {
#ifndef NDEBUG
	Serial.print(t);
#else
	(void)t;
#endif // NDEBUG
}

void setup() {
#ifndef NDEBUG
	Serial.begin(serial_baud);
#endif // NDEBUG

	Wire.begin();
	Wire.setClock(clock_frequency_I2C);

	pinMode(photo_pin, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(photo_pin), photo_interrupt, FALLING);

	accelgyro.initialize();

	// see if the card is present and can be initialized:
	if (SDLib::SD.begin(chipselect_pin)) {
		serial_println("SD ok");

		logfile = SDLib::SD.open("gravity.txt", FILE_WRITE);
	} else {
		serial_println("SD failed");

		while (true) {}
	}

	log_startup_event(millis(), logfile);
	logfile.flush();
}

// We don't calibrate the data on the Arduino itself (mostly
// because I don't feel like implementing this without having
// Eigen at hand), instead we just store the raw data from the
// sensor to the SD card.
void get_acc(gravity_vector_t &vec) {
	accelgyro.getAcceleration(&vec[0], &vec[1], &vec[2]);
	// serial_print(vec[0]);
	// serial_print(' ');
	// serial_print(vec[1]);
	// serial_print(' ');
	// serial_println(vec[2]);
}

void loop() {
	const unsigned long millis_at_iter_start = millis();

	// Record gravity vectors
	if (millis_at_iter_start > gravity_state.next_record_time) {
		gravity_state.next_record_time =
		    millis_at_iter_start + gravity_record_delay;
		get_acc(gravity_state.buffer.push());
	}

	// Handle a photo event -> write info to SD
	if (photo_state.pending) {
		// Write gravity vectors and current time to sd. The whole
		// writing and flushing together seems to take between 18
		// and 24 milliseconds.
		for (unsigned long i = 0; i < gravity_state.buffer.size(); ++i)
			log_gravity_vector(gravity_state.buffer[i], logfile);
		log_photo_event(photo_state.time, logfile);

		photo_state.pending = false;

		logfile.flush();

		gravity_state.buffer.clear();
		gravity_state.next_record_time = 0;
	}
}
