cmake_minimum_required(VERSION 3.14)
project(arduino-gravity-recorder-tests LANGUAGES CXX)

# All of this here is a bit dodgy because technically we're not testing
# the same library as what will be running on the arduino in the end, at
# least from a CMake perspective. Catch2 depends on the standard library
# which we don't have available with the arduino toolchain, so this
# CMake lists compiles the tests using the normal toolchain provided by
# CMake.

include(FetchContent)
FetchContent_Declare(
	Catch2
	GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	GIT_TAG        v2.12.1
)
FetchContent_MakeAvailable(Catch2)

add_executable(tests
	"${CMAKE_CURRENT_LIST_DIR}/test.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/cyclic_buffer_test.cpp"
)
target_link_libraries(tests PRIVATE Catch2)
target_include_directories(tests PRIVATE
	"${CMAKE_CURRENT_LIST_DIR}/../src"
)
