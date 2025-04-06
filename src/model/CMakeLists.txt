add_library(model Model.cpp Mesh.cpp WorldObject.cpp)

target_include_directories(model INTERFACE
    "${CMAKE_SOURCE_DIR}/include"
)
