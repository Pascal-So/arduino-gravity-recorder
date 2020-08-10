#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <SD.h>

#include "log_entry_types.hpp"

// If this starts getting out of hand then maybe replace
// it with protobuf: https://github.com/nanopb/nanopb
// should be fine for now though.

// The ATmega328 is little endian so we can decode it on
// x86_64 without worrying about endianness.

void write_log_entry_type(log_entry_types entry_type, SDLib::File &file) {
	file.write(static_cast<uint8_t>(entry_type));
}

void log_gravity_vector(const gravity_vector_t &vec, SDLib::File &file) {
	write_log_entry_type(log_entry_types::gravity_vector, file);
	file.write(reinterpret_cast<const uint8_t *>(vec), sizeof(int16_t) * 3);
}

void log_photo_event(unsigned long timestamp, SDLib::File &file) {
	write_log_entry_type(log_entry_types::photo_event, file);
	file.write(reinterpret_cast<const uint8_t *>(&timestamp),
	           sizeof(unsigned long));
}

void log_startup_event(unsigned long timestamp, SDLib::File &file) {
	write_log_entry_type(log_entry_types::startup, file);
	file.write(reinterpret_cast<const uint8_t *>(&timestamp),
	           sizeof(unsigned long));
}

#endif // LOGGER_HPP
