cmake_minimum_required(VERSION 3.14)
project(recording_parser LANGUAGES CXX)

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /DNDEBUG")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -ggdb -fno-omit-frame-pointer -D_GLIBCXX_DEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -DNDEBUG")
endif()


## fetch / find dependencies

include(FetchContent)

FetchContent_Declare(
	libfmt
	GIT_REPOSITORY https://github.com/fmtlib/fmt.git
	GIT_TAG        7.0.3
)
FetchContent_MakeAvailable(libfmt)

FetchContent_Declare(
	program_options
	GIT_REPOSITORY https://github.com/Fytch/ProgramOptions.hxx
	GIT_TAG        159cb7d4265b1de6b1fd4fb2fafa8123c2a0f69a
)
set(PROGRAMOPTIONS_BUILD_TEST     CACHE BOOL FALSE)
set(PROGRAMOPTIONS_BUILD_EXAMPLES CACHE BOOL FALSE)
FetchContent_MakeAvailable(program_options)

find_package(Eigen3 3.3 REQUIRED NO_MODULE)


## create executable

add_executable(recording_parser "${CMAKE_CURRENT_LIST_DIR}/recording_parser.cpp")
target_link_libraries(recording_parser
	Eigen3::Eigen
	ProgramOptionsHxx
	fmt
)
