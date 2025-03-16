add_library(glad glad.c)

# PUBLIC instead of INTERFACE because it's a C file instead of C++
target_include_directories(glad PUBLIC
    "${CMAKE_SOURCE_DIR}/include"
)
