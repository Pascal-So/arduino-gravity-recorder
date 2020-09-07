#include "calibrate.hpp"

#include "parse.hpp"

#include <Eigen/SVD>

#include <iostream>
#include <stdexcept>

// Assuming that the photos have been distributed roughly evenly around the
// circle and that the rotation axis was close to the global vertical axis.
// The returned vector is the camera space +y vector in IMU coordinates. y is
// down on the image.
Eigen::Vector3f get_y_axis(std::vector<Eigen::Vector3f> const& photo_gravity) {
	Eigen::Vector3f vertical_axis = Eigen::Vector3f::Zero();

	for (auto const& g : photo_gravity)
		vertical_axis += g;

	return vertical_axis.normalized();
}

// Assumes that the camera x axis was roughly orthogonal to the world vertcal
// axis and the photos were taken while rotating exactly around the camera x
// axis.
Eigen::Vector3f get_x_axis(std::vector<Eigen::Vector3f> const& photo_gravity) {
	// Simple linear regression
	const int n = photo_gravity.size();

	assert(n >= 4);

	Eigen::MatrixXf m(n, 4);
	for (int i = 0; i < n; ++i) {
		m.block<1, 3>(i, 0) = photo_gravity[i].transpose().normalized();
	}

	m.col(3) = Eigen::VectorXf::Ones(n);

	// We find a plane through the data. The first three components are
	// the normal of the plane, the last component is the offset.
	Eigen::Vector4f w = m.bdcSvd(Eigen::ComputeFullV).matrixV().col(3);
	w /= w.head<3>().norm();

	Eigen::Vector3f normal = w.head<3>();

	// Here we make sure that the rotation axis was close to orthogonal
	// to gravity. The offset of the plane is equal the sin of the
	// angle between the rotation axis and a perfectly horizontal line.
	const float sin_angle = w(3);
	assert(std::abs(sin_angle) < 0.2);

	// We check if the camera was rotated in positive or negative
	// direction around `normal` by checking how the first and last
	// vector split up the circle into sectors and seeing into which
	// of the sectors the middle recording falls.

	std::array<Eigen::Vector3f, 3> vecs_on_plane = {
	    m.block<1, 3>(0, 0), m.block<1, 3>(n / 2, 0), m.block<1, 3>(n - 1, 0)};

	for (auto& v : vecs_on_plane)
		v -= v.dot(normal) * normal;

	const Eigen::Vector3f centre_of_positive_sector =
	    normal.cross(vecs_on_plane[0]) - normal.cross(vecs_on_plane[2]);

	if (centre_of_positive_sector.dot(vecs_on_plane[1]) <
	    centre_of_positive_sector.dot(vecs_on_plane[0]))
		normal *= -1;

	return normal;
}

// Returns the rotation from IMU frame to Camera frame.
Eigen::Quaternionf calibrate(std::ifstream& recording_infile,
                             int x_session_index, int y_session_index,
                             int nr_skip_measurements) {
	int session_index = -1;

	bool x_calibrated = false;
	bool y_calibrated = false;

	if (x_session_index == y_session_index) {
		throw std::invalid_argument(
		    "The indices for calibrating the x and y axes can't be equal.");
	}

	std::vector<Eigen::Vector3f> measurements;
	measurements.reserve(30);

	std::vector<Eigen::Vector3f> photo_gravities;

	Eigen::Vector3f x_axis;
	Eigen::Vector3f y_axis;

	const auto run_calibrations = [&]() {
		if (session_index == x_session_index) {
			x_axis = get_x_axis(photo_gravities);
			x_calibrated = true;
		} else if (session_index == y_session_index) {
			y_axis = get_y_axis(photo_gravities);
			y_calibrated = true;
		}
	};

	log_entry_types entry_type;
	while (read_next_entry(recording_infile, entry_type)) {
		switch (entry_type) {
		case log_entry_types::photo_event:
			parse_photo(recording_infile).millis;

			if (session_index >= 0 && (session_index == x_session_index ||
			                           session_index == y_session_index)) {
				Eigen::Vector3f g = Eigen::Vector3f::Zero();
				int nr_relevant_measurements =
				    measurements.size() - nr_skip_measurements;
				for (int i = 0; i < nr_relevant_measurements; ++i)
					g += measurements[i];
				g /= nr_relevant_measurements;

				photo_gravities.push_back(g);
			}

			measurements.clear();
			break;
		case log_entry_types::startup:

			if (session_index >= 0)
				run_calibrations();

			parse_startup(recording_infile);
			photo_gravities.clear();
			measurements.clear();
			++session_index;
			break;
		case log_entry_types::gravity_vector:
			measurements.push_back(parse_gravity(recording_infile).g);
			break;
		}
	}

	if (session_index >= 0)
		run_calibrations();

	// If only one of the axes was calibrated we still want to print the
	// result, this is useful for testing.
	if (x_calibrated)
		std::cout << "Sensor x axis in IMU coordinates: " << x_axis.transpose()
		          << '\n';
	if (y_calibrated)
		std::cout << "Sensor y axis in IMU coordinates: " << y_axis.transpose()
		          << '\n';

	if (x_calibrated && y_calibrated) {
		// running a complete calibration

		Eigen::Matrix3f rotation_matrix;
		rotation_matrix << x_axis, y_axis, x_axis.cross(y_axis);

		// Invert the rotation because we want to get the conversion from IMU
		// frame to camera.
		return Eigen::Quaternionf(rotation_matrix).conjugate();
	} else {
		if (x_session_index >= session_index)
			throw std::runtime_error("Invalid x session index.");
		if (y_session_index >= session_index)
			throw std::runtime_error("Invalid y session index.");
	}

	throw std::runtime_error("Need both x and y for complete calibration.");
}
