#ifndef CALIBRATE_HPP
#define CALIBRATE_HPP

#include <vector>

#include <Eigen/Geometry>

// Assuming that the photos have been distributed roughly evenly around the
// circle and that the rotation axis was close to the global vertical axis.
// The returned vector is the camera space +y vector in IMU coordinates. y is down on the image.
Eigen::Vector3f get_y_axis(std::vector<Eigen::Vector3f> const& photo_gravity);

// Assumes that the camera x axis was roughly orthogonal to the world vertcal axis and
// the photos were taken while rotating exactly around the camera x axis.
Eigen::Vector3f get_x_axis(std::vector<Eigen::Vector3f> const& photo_gravity);

// Returns the rotation from IMU frame to Camera frame.
Eigen::Quaternionf calibrate(std::ifstream& recording_infile,
                             int x_session_index, int y_session_index,
                             int nr_skip_measurements);

#endif // CALIBRATE_HPP
