cmake_minimum_required(VERSION 3.11)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")
include(SetupArduinoToolchain)

project(arduino_gravity_recorder LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++2a")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -s")
set(CMAKE_C_FLAGS_RELEASE "-DNDEBUG -s")

include(GetDependencies)

add_executable(arduino_gravity_recorder "${CMAKE_CURRENT_LIST_DIR}/arduino_gravity_recorder.cpp")

target_link_libraries(arduino_gravity_recorder PRIVATE grove)

target_link_arduino_libraries(arduino_gravity_recorder PRIVATE core SD)

# upload with `make upload-arduino_gravity_recorder SERIAL_PORT=/dev/ttyUSB0`
target_enable_arduino_upload(arduino_gravity_recorder)
