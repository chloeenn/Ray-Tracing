cmake_minimum_required(VERSION 3.10)

# Project Name
project(RayTracer)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add include directory for GLM
include_directories(${CMAKE_SOURCE_DIR}/glm-master)

# Define source files
set(SOURCES
    raytracer.cpp
    invert.cpp
    ppm.cpp
    InputReader.cpp
)

# Add executable
add_executable(raytracer ${SOURCES})

