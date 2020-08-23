#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

#include <Eigen/Dense>
#include <ProgramOptions.hxx>
#include <fmt/core.h>

#include "parse.hpp"
#include "calibrate.hpp"

void list_sessions(std::ifstream& recording_infile) {
	log_entry_types entry_type;

	int session_index = 0;
	int photos_in_session = 0;
	std::uint32_t last_photo_time_in_session;

	while (read_next_entry(recording_infile, entry_type)) {
		switch (entry_type) {
		case log_entry_types::photo_event:
			last_photo_time_in_session = parse_photo(recording_infile).millis;
			++ photos_in_session;
			break;
		case log_entry_types::startup:
			parse_startup(recording_infile);

			fmt::print("{:8} - {:3} photos", session_index, photos_in_session);
			if (photos_in_session > 0)
				fmt::print(" in {:4} seconds\n", last_photo_time_in_session / 1000);
			else
				fmt::print("\n");

			photos_in_session = 0;
			last_photo_time_in_session = 0;
			++ session_index;
			break;
		case log_entry_types::gravity_vector:
			parse_gravity(recording_infile);
			break;
		}
	}
}

int main(int argc, char **argv) {
	po::parser parser;

	auto& help = parser["help"]
		.abbreviation('h')
		.description("print this help screen");

	std::string recording_path = "/run/media/pascal/ARDUINO/GRAVITY.TXT";
	parser["file"]
		.bind(recording_path)
		.abbreviation('f')
		.description("path to the recorded GRAVITY.TXT");

	int nr_of_ignored_measurements = 5;
	parser["ignore"]
		.bind(nr_of_ignored_measurements)
		.abbreviation('i')
		.description("number of gravity measurements to drop just before every photo. ideal value depends on camera and settings (default: 5)");

	unsigned x_session_index;
	auto& x_parser = parser["x-session-index"]
		.bind(x_session_index)
		.abbreviation('x')
		.description("index of session to use for calibrating x axis. See --list for indices.");

	unsigned y_session_index;
	auto& y_parser = parser["y-session-index"]
		.bind(y_session_index)
		.abbreviation('y')
		.description("index of session to use for calibrating y axis. See --list for indices.");

	int nr_skip_measurements = 5;
	parser["nr-skip-measurements"]
		.bind(nr_skip_measurements)
		.abbreviation('s');

	auto& list = parser["list"]
		.abbreviation('l')
		.description("List all sessions in the given recording file and exit.");



	if (!parser(argc, argv))
		return EXIT_FAILURE;

	if (help.was_set()) {
		std::cerr << parser << '\n';
		return EXIT_SUCCESS;
	}

	std::ifstream recording_infile(recording_path, std::ios_base::binary);
	if (!recording_infile) {
		std::cerr << "Could not find recorded file.\n";
		return EXIT_FAILURE;
	}

	if (list.was_set()) {
		list_sessions(recording_infile);
		return EXIT_SUCCESS;
	}

	if (!x_parser.was_set())
		throw std::invalid_argument("x index missing");

	if (!y_parser.was_set())
		throw std::invalid_argument("y index missing");

	const Eigen::Quaternionf calib = calibrate(recording_infile, x_session_index, y_session_index, nr_skip_measurements);
	std::cout << calib.vec().transpose() << ' ' << calib.w() << '\n';
}
