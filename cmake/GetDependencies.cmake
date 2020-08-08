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
