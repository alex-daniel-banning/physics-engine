add_library(model Model.cpp Mesh.cpp)

target_include_directories(model INTERFACE
    "${CMAKE_SOURCE_DIR}/include"
)
