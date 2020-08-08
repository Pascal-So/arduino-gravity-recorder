#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <Wire.h>

#include <I2Cdev.h>
#include <MPU9250.h>

constexpr long serial_baud = 9600;
constexpr uint8_t chipselect_pin = 10;
constexpr uint32_t clock_frequency_I2C = 400000; // 400kHz

MPU9250 accelgyro;
int nr_contacts;
unsigned long nr_iterations;
unsigned long last_time;
unsigned long start_photo_time;

void setup() {
	Serial.begin(serial_baud);

	Wire.begin();
	Wire.setClock(clock_frequency_I2C);

	pinMode(2, INPUT_PULLUP);

	nr_contacts = 0;
	nr_iterations = 0;
	last_time = 0;

	// accelgyro.initialize();

	// see if the card is present and can be initialized:
	// if (!SDLib::SD.begin(chipselect_pin)) {
	// 	Serial.println("Card failed or not present");
	// 	while(1);
	// }
}

void get_acc() {
	// int16_t acc_x, acc_y, acc_z;
	// accelgyro.getAcceleration(&acc_x, &acc_y, &acc_z);

	// Serial.print(acc_x);
	// Serial.print(' ');
	// Serial.print(acc_y);
	// Serial.print(' ');
	// Serial.println(acc_z);
}


void loop() {
	// SDLib::File logfile = SDLib::SD.open("test.txt", FILE_WRITE);
	// logfile.close();

	// get_acc();

	++nr_iterations;

	const bool contact = digitalRead(2) == LOW;

	if (contact) {
		if (nr_contacts == 0) {
			start_photo_time = millis();
		}
		nr_contacts++;
	}
	if (!contact && nr_contacts > 0) {
		Serial.println("contact time");
		Serial.println(millis() - start_photo_time);
		nr_contacts = 0;
		// delay(25); // ms
	}

	if (nr_iterations % 100000 == 0) {
		const auto current_time = millis();
		const auto diff = current_time - last_time;
		last_time = current_time;

		Serial.println("millis for 100000 iterations:");
		Serial.println(diff);
	}
}
