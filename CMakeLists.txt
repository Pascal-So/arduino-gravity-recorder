cmake_minimum_required(VERSION 3.11)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")
include(SetupArduinoToolchain)

project(arduino_gravity_recorder LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++2a")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -s")
set(CMAKE_C_FLAGS_RELEASE "-DNDEBUG -s")

include(FetchContent)
FetchContent_Declare(
	grove
	GIT_REPOSITORY https://github.com/Seeed-Studio/Grove_IMU_10DOF_v2.0
	GIT_TAG        d4d0ff6879bbfdcafbbd096e9446bde295b026aa
)

FetchContent_GetProperties(grove)
if(NOT grove_POPULATED)
	FetchContent_Populate(grove)
	add_library(grove
		"${grove_SOURCE_DIR}/BMP280.cpp"
		"${grove_SOURCE_DIR}/I2Cdev.cpp"
		"${grove_SOURCE_DIR}/MPU9250.cpp"
	)

	target_include_directories(grove PUBLIC "${grove_SOURCE_DIR}")
	target_link_arduino_libraries(grove PUBLIC core Wire)
endif()

add_executable(arduino_gravity_recorder
	"${CMAKE_CURRENT_LIST_DIR}/src/arduino_gravity_recorder.cpp"
)
target_link_libraries(arduino_gravity_recorder PRIVATE grove)
target_link_arduino_libraries(arduino_gravity_recorder PRIVATE core SD)

# upload with `make upload-arduino_gravity_recorder SERIAL_PORT=/dev/ttyUSB0`
target_enable_arduino_upload(arduino_gravity_recorder)
