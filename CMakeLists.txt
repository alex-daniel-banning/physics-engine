cmake_minimum_required(VERSION 3.10)

project(physics-engine VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(src/glad)

add_executable(engine src/main.cpp)

target_link_libraries(engine PUBLIC glad)

# Make sure CMake knows about your include directory
target_include_directories(engine PUBLIC
    "${CMAKE_SOURCE_DIR}/include"
)

