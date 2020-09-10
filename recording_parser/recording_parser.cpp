#include "calibrate.hpp"
#include "extract.hpp"
#include "parse.hpp"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <Eigen/Geometry>
#include <ProgramOptions.hxx>
#include <fmt/core.h>

Eigen::Quaternionf parse_calib(std::string const& calib) {
	Eigen::Quaternionf quat;

	std::istringstream ss(calib);
	int index = 0;
	float f;
	while (ss >> f) {
		// Eigen stores quaternion as xyzw internally for some reason
		quat.coeffs()((index + 3) % 4) = f;
		if (ss.peek() == ',')
			ss.ignore();

		++index;
		if (index >= 4)
			break;
	}

	ss.peek();
	if (index != 4 || !ss.eof()) {
		throw std::invalid_argument(
		    "Invalid calib string. Should be 4 comma separated floats.");
	}

	return quat;
}

std::string serialize_calib(Eigen::Quaternionf const& calib) {
	return fmt::format("{:.6f},{:.6f},{:.6f},{:.6f}",
	                   calib.w(), calib.x(), calib.y(), calib.z());
}

void show_session_detail(std::ifstream& recording_infile,
                         int show_session_index, int nr_skip_measurements) {
	log_entry_types entry_type;

	int session_index = -1;
	int photos_in_session = 0;
	std::uint32_t last_photo_time = 0;

	std::vector<Eigen::Vector3f> measurements;
	measurements.reserve(26);

	while (read_next_entry(recording_infile, entry_type)) {
		switch (entry_type) {
		case log_entry_types::photo_event: {
			const auto photo_time = parse_photo(recording_infile).millis;
			if (session_index == show_session_index) {
				fmt::print("{: >5d}   {: >12d}  {: >9.2f}", photos_in_session,
				           measurements.size(), photo_time / 1000.);

				if (photos_in_session > 0)
					fmt::print("   {: >19.2f}    ",
					           (photo_time - last_photo_time) / 1000.);
				else
					fmt::print("   {: >19}    ", "-");

				Eigen::Vector3f g = Eigen::Vector3f::Zero();
				const int nr_relevant_measurements =
				    measurements.size() - nr_skip_measurements;
				for (int i = 0; i < nr_relevant_measurements; ++i)
					g += measurements[i];
				g.normalize();

				for (int i = 0; i < 3; ++i) {
					fmt::print("{: >7.3f}", g(i));
				}
				fmt::print("\n");

				last_photo_time = photo_time;
				++photos_in_session;
			}
			measurements.clear();
			break;
		}
		case log_entry_types::startup: {
			parse_startup(recording_infile);
			photos_in_session = 0;
			last_photo_time = 0;
			measurements.clear();
			++session_index;
			if (session_index == show_session_index) {
				fmt::print("Session {0}\nphoto   measurements   time [s]   "
				           "time since last [s]   normalized avg gravity\n",
				           show_session_index);
			}
			break;
		}
		case log_entry_types::gravity_vector:
			measurements.push_back(parse_gravity(recording_infile).g);
			break;
		}

		if (session_index > show_session_index)
			break;
	}
}

void list_sessions(std::ifstream& recording_infile) {
	log_entry_types entry_type;

	int session_index = -1;
	int photos_in_session = 0;
	std::uint32_t last_photo_time_in_session;

	const auto print = [&]() {
		fmt::print("{: >7d}   {: >6d}", session_index, photos_in_session);
		if (photos_in_session > 0)
			fmt::print("  {: >9.2f}\n", last_photo_time_in_session / 1000.);
		else
			fmt::print("\n");
	};

	fmt::print("session   photos   time [s]\n");
	while (read_next_entry(recording_infile, entry_type)) {
		switch (entry_type) {
		case log_entry_types::photo_event:
			last_photo_time_in_session = parse_photo(recording_infile).millis;
			++photos_in_session;
			break;
		case log_entry_types::startup:
			parse_startup(recording_infile);

			if (session_index >= 0) {
				print();
			}

			photos_in_session = 0;
			last_photo_time_in_session = 0;
			++session_index;
			break;
		case log_entry_types::gravity_vector:
			parse_gravity(recording_infile);
			break;
		}
	}

	if (session_index >= 0) {
		print();
	}
}

int main(int argc, char **argv) {
	po::parser parser;

	auto& help_option = parser["help"]
	    .abbreviation('h')
	    .description("Print this help screen");

	std::string recording_path = "/run/media/pascal/ARDUINO/GRAVITY.TXT";
	parser["file"]
	    .bind(recording_path)
	    .abbreviation('f')
	    .description("Path to the recorded 'GRAVITY.TXT' file.");

	int nr_of_ignored_measurements = 5;
	parser["ignore"]
	    .bind(nr_of_ignored_measurements)
	    .abbreviation('i')
	    .description("Number of gravity measurements to drop just before every "
	                 "photo. The ideal value depends on the camera model and "
	                 "settings (default: 5).");

	int x_session_index = -1;
	auto& x_session_index_option = parser["x-session-index"]
	    .bind(x_session_index)
	    .abbreviation('x')
	    .description(
	        "Index of session to use for calibrating x axis. See --list for "
	        "indices.");

	int y_session_index = -1;
	auto& y_session_index_option = parser["y-session-index"]
	    .bind(y_session_index)
	    .abbreviation('y')
	    .description(
	        "Index of session to use for calibrating y axis. See --list "
	        "for indices.");

	auto& list_option = parser["list"]
	    .abbreviation('l')
	    .description("List all sessions in the recording file and exit.");

	int show_session_index;
	auto& show_session_option = parser["session"]
	    .bind(show_session_index)
	    .abbreviation('s')
	    .description("Show this session in detail and exit.");

	std::string calibration = "1,0,0,0";
	parser["calibration"]
	    .bind(calibration)
	    .abbreviation('c')
	    .description(
	        "Calibration to apply to the measurements in extract-session.");

	int extract_session_index;
	auto& extract_session_option = parser["extract-session"]
	    .bind(extract_session_index)
	    .abbreviation('e')
	    .description("Extract the data from this session. Needs to be used "
	                 "together with extract-destination.");

	std::string extract_destination;
	auto& extract_destination_option = parser["extract-destination"]
	    .bind(extract_destination)
	    .abbreviation('d')
	    .description("Path to which to extract the gravity vectors. Needs to "
	                 "be used together with extract-session.");

	if (!parser(argc, argv))
		return EXIT_FAILURE;

	if (help_option.was_set()) {
		std::cerr << parser << '\n';
		return EXIT_SUCCESS;
	}

	std::ifstream recording_infile(recording_path, std::ios_base::binary);
	if (!recording_infile) {
		std::cerr << "Could not find recorded file.\n";
		return EXIT_FAILURE;
	}

	if (list_option.was_set()) {
		std::cout << "Listing all sessions in " << recording_path << "\n\n";

		list_sessions(recording_infile);
		return EXIT_SUCCESS;
	}

	if (show_session_option.was_set()) {
		show_session_detail(recording_infile, show_session_index,
		                    nr_of_ignored_measurements);
		return EXIT_SUCCESS;
	}

	if (x_session_index_option.was_set() || y_session_index_option.was_set()) {
		std::cout << "Running Calibration\n\n";

		const Eigen::Quaternionf computed_calib =
		    calibrate(recording_infile, x_session_index, y_session_index,
		              nr_of_ignored_measurements);

		std::cout << "Calibration: " << serialize_calib(computed_calib) << '\n';

		return EXIT_SUCCESS;
	}

	if (extract_session_option.was_set() &&
	    extract_destination_option.was_set()) {
		std::cout << "Extracting data from session " << extract_session_index
		          << "\n\n";

		extract(recording_infile, extract_session_index, extract_destination,
		        nr_of_ignored_measurements, parse_calib(calibration));

		return EXIT_SUCCESS;
	}

	std::cerr << parser << '\n';
}
