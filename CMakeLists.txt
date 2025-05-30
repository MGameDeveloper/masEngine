#
cmake_minimum_required(VERSION 4.0)

# Define the name, version, and supported programming language of the project
project(masEngine VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Create and executable target
add_executable(masEngine)

# Adds source files to a target
target_sources(masEngine PRIVATE src/*.cpp)

# Specifies include directories for a target
target_include_directories(masEngine PRIVATE inc)

# Enable compiler warnings (optional, but recommended)
target_compile_options(masEngine PRIVATE
    -Wall
	-Wextra
	-Wpedantic
)