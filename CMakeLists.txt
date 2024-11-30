cmake_minimum_required(VERSION 3.10)

# Project name
project(CameraApp)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find OpenCV package
find_package(OpenCV REQUIRED)

# Find nlohmann/json package
find_package(nlohmann_json REQUIRED)

# Add the executable
add_executable(CameraApp main.cpp Camera.hpp Image.hpp)

# Link libraries
target_link_libraries(CameraApp ${OpenCV_LIBS} nlohmann_json::nlohmann_json pthread)

# Display the OpenCV version being used
message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "nlohmann/json included.")
