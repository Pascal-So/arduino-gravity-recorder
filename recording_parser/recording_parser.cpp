#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

using std::int16_t;
using std::int8_t;
#include "../src/log_entry_types.hpp"

#include <Eigen/Dense>

int main(int argc, char **argv) {
	std::vector<gravity_vector_t> gravity_vectors;

	if (argc != 2) {
		std::cerr << "usage: " << argv[0] << " recording_filename\n";
		return 2;
	}

	const std::string filename = argv[1];
	std::ifstream infile(filename, std::ios_base::binary);

	log_entry_types entry_type;
	while (infile.read(reinterpret_cast<char *>(&entry_type),
	                   sizeof(entry_type))) {
		switch (entry_type) {
		case log_entry_types::gravity_vector: {
			gravity_vector_t int_vec;
			infile.read(reinterpret_cast<char *>(&int_vec), sizeof(int_vec));

			std::cout << "int vec "
				<< int_vec[0] << ' '
				<< int_vec[1] << ' '
				<< int_vec[2] << "\n";

			break;
		}
		case log_entry_types::photo_event: {
			std::uint32_t millis;
			infile.read(reinterpret_cast<char *>(&millis), sizeof(millis));
			std::cout << "photo at " << millis << "ms.\n";
			break;
		}
		case log_entry_types::startup: {
			std::uint32_t millis;
			infile.read(reinterpret_cast<char *>(&millis), sizeof(millis));
			std::cout << "startup done after " << millis << "ms.\n";
			break;
		}
		}
	}
}
