#
cmake_minimum_required(VERSION 4.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define the name, version, and supported programming language of the project
project(masEngine VERSION 1.0 LANGUAGES CXX)

# Create and executable target
add_executable(masEngine)

# Adds source files to a target
target_sources(masEngine PRIVATE 
    src/masEngine.cpp
	src/masTimeWin.cpp
	src/masWindowWin.cpp
    src/masInputControllerWin.cpp
	src/Test.cpp
)

# Created to make visual studio IDE recognize header files
#target_sources(masEngine PRIVATE
#    FILE_SET HEADERS
#    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
#    FILES
#        inc/masEngine.h
#        inc/masTime.h
#        inc/masWindow.h
#        inc/masInputController.h
#)

# Specifies include directories for a target
target_include_directories(masEngine PRIVATE inc)

#
add_compile_definitions(UNICODE _UNICODE)

# Enable compiler warnings (optional, but recommended)
#target_compile_options(masEngine PRIVATE)