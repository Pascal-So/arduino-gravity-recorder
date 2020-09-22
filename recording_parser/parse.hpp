#ifndef PARSE_HPP
#define PARSE_HPP

#include <fstream>
#include <cstdint>

#include <Eigen/Geometry>

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

inline startup_t parse_startup(std::ifstream& recording_infile) {
	std::uint32_t millis;
	recording_infile.read(reinterpret_cast<char *>(&millis), sizeof(millis));
	return {millis};
}
inline photo_t parse_photo(std::ifstream& recording_infile) {
	std::uint32_t millis;
	recording_infile.read(reinterpret_cast<char *>(&millis), sizeof(millis));
	return {millis};
}
inline gravity_t parse_gravity(std::ifstream& recording_infile) {
	Eigen::Matrix<int16_t, 3, 1> int_vec;
	recording_infile.read(reinterpret_cast<char *>(int_vec.data()), sizeof(int16_t) * 3);

	// The stored data is acceleration but we need the force which
	// is in the opposite direction.
	return {int_vec.cast<float>() * -1};
}

inline bool read_next_entry(std::ifstream& recording_infile,
                            log_entry_types& entry_type) {
	recording_infile.read(reinterpret_cast<char *>(&entry_type), sizeof(entry_type));
	return recording_infile.good();
}

#endif // PARSE_HPP
