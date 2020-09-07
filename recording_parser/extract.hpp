#ifndef EXTRACT_HPP
#define EXTRACT_HPP

#include <fstream>
#include <string>

#include <Eigen/Geometry>

void extract(std::ifstream& recording_infile, int extract_session_index,
             std::string const& extract_destination, int nr_skip_measurements,
             Eigen::Quaternionf const& calibration);

#endif // EXTRACT_HPP
