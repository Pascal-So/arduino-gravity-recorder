#include "extract.hpp"

#include "parse.hpp"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <stdexcept>

namespace fs = std::filesystem;

void extract(std::ifstream& recording_infile, int extract_session_index,
             std::string const& extract_destination, int nr_skip_measurements,
             Eigen::Quaternionf const& calibration) {
	const fs::path extract_path(extract_destination);

	int nr_photos_extracted = 0;

	if (!fs::is_directory(extract_path)) {
		throw std::invalid_argument("Path " + extract_destination +
		                            " does not exist.");
	}

	std::vector<fs::path> files;
	for (const auto& file : fs::directory_iterator(extract_path)) {
		if (!file.is_regular_file())
			continue;

		const auto path = file.path();

		if (path.extension() == ".txt" || path.extension() == "")
			continue;

		files.push_back(path.filename());
	}

	if (files.empty())
		throw std::invalid_argument("Directory " + extract_destination +
		                            " doesn't contain any photos.");

	std::cout << "Directory contains " << files.size() << " photos.\n";

	// We assume that the files have ascending names, which might be wrong if
	// the file numbering of the camera resets. A smarter way would be to
	// compare the timestamps of the photos to the times of the measurements,
	// might do that at some later point.
	std::sort(files.begin(), files.end());

	int session_index = -1;
	unsigned photo_index = 0;

	const Eigen::Matrix3f calib_rotation(calibration);

	std::cout << "Using calibration:\n" << calib_rotation << "\n\n";

	std::vector<Eigen::Vector3f> measurements;
	measurements.reserve(26);

	log_entry_types entry_type;
	while (read_next_entry(recording_infile, entry_type)) {
		switch (entry_type) {
		case log_entry_types::photo_event:
			parse_photo(recording_infile).millis;

			if (session_index == extract_session_index) {
				if (photo_index >= files.size()) {
					throw std::runtime_error(
					    "Too many measurements, not enough photos.");
				}

				Eigen::Vector3f g = Eigen::Vector3f::Zero();
				int nr_relevant_measurements =
				    measurements.size() - nr_skip_measurements;
				for (int i = 0; i < nr_relevant_measurements; ++i)
					g += measurements[i];
				g.normalize();
				g = calib_rotation * g;

				const auto txt_path =
				    extract_path / files[photo_index].replace_extension(".txt");

				std::ofstream os(txt_path, std::ios::trunc);
				os << g(0) << ' ' << g(1) << ' ' << g(2) << '\n';

				++nr_photos_extracted;
			}

			++photo_index;

			measurements.clear();
			break;
		case log_entry_types::startup:
			parse_startup(recording_infile);
			++session_index;
			measurements.clear();
			photo_index = 0;
			break;
		case log_entry_types::gravity_vector:
			measurements.push_back(parse_gravity(recording_infile).g);
			break;
		}
	}

	if (extract_session_index > session_index)
		throw std::invalid_argument("Invalid session index.");

	std::cout << "Extracted data for " << nr_photos_extracted << " photos.\n";
}
