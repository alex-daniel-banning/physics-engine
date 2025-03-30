cmake_minimum_required(VERSION 3.10)

project(physics-engine VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(src/glad)

file(GLOB MODEL_SOURCES "src/model/*.cpp")
add_executable(engine src/main.cpp src/stb_image.cpp ${MODEL_SOURCES})

# Make sure CMake knows about your include directory
target_include_directories(engine PUBLIC
    "${CMAKE_SOURCE_DIR}/include"
)
# Had to build /usr/local/lib/libglfw.so
target_link_libraries(engine PUBLIC glad glfw assimp)


