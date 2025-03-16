cmake_minimum_required(VERSION 3.10)

project(physics-engine VERSION 1.0)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Create the executable (Note the corrected path to src/main.cpp)
add_executable(engine src/main.cpp)

# Make sure CMake knows about your include directory
target_include_directories(engine PUBLIC
    "${CMAKE_SOURCE_DIR}/include"
)

