#ifndef PARSE_HPP
#define PARSE_HPP

#include <fstream>
#include <cstdint>

#include <Eigen/Dense>

using std::int16_t;
using std::int8_t;
#include "../src/log_entry_types.hpp"

struct startup_t {
	std::uint32_t millis;
};
struct photo_t {
	std::uint32_t millis;
};
struct gravity_t {
	Eigen::Vector3f g;
};

startup_t parse_startup(std::ifstream& recording_infile) {
	std::uint32_t millis;
	recording_infile.read(reinterpret_cast<char *>(&millis), sizeof(millis));
	return {millis};
}
photo_t parse_photo(std::ifstream& recording_infile) {
	std::uint32_t millis;
	recording_infile.read(reinterpret_cast<char *>(&millis), sizeof(millis));
	return {millis};
}
gravity_t parse_gravity(std::ifstream& recording_infile) {
	Eigen::Matrix<int16_t, 3, 1> int_vec;
	recording_infile.read(reinterpret_cast<char *>(int_vec.data()), sizeof(int16_t) * 3);
	return {int_vec.cast<float>()};
}

bool read_next_entry(std::ifstream& recording_infile, log_entry_types& entry_type) {
	recording_infile.read(reinterpret_cast<char *>(&entry_type), sizeof(entry_type));
	return recording_infile.good();
}

#endif // PARSE_HPP
