# Use the Arduino Toolchain in this CMake project. Has to be included
# before the call to `project(...)`. Requires CMake 3.11
# Pascal Sommer, 2020

include(FetchContent)

if(NOT DEFINED ARDUINO_TOOLCHAIN_PATH)
	FetchContent_Declare(
		arduino
		GIT_REPOSITORY https://github.com/a9183756-gh/Arduino-CMake-Toolchain
		GIT_TAG        e745a9bed3c3fb83442d55bf05630f31574674f2
	)

	FetchContent_GetProperties(arduino)
	if(NOT arduino_POPULATED)
		FetchContent_Populate(arduino)
		set(ARDUINO_TOOLCHAIN_PATH "${arduino_SOURCE_DIR}")
	endif()
endif()

set(CMAKE_TOOLCHAIN_FILE "${ARDUINO_TOOLCHAIN_PATH}/Arduino-toolchain.cmake")
set(ARDUINO_BOARD_OPTIONS_FILE "${CMAKE_CURRENT_LIST_DIR}/BoardOptions.cmake")
