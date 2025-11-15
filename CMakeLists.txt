cmake_minimum_required(VERSION 3.10)

# Project name and version
project(proj VERSION 1.0)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include header files
include_directories(include)

# Add source files
file(GLOB SOURCES "src/*.cpp")

# Create the executable
add_executable(proj ${SOURCES})