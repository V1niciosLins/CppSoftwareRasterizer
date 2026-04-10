cmake_minimum_required(VERSION 3.20)

project(SoftwareRasterizer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(rasterizer src/main.cpp src/Framebuffer.cpp src/mesh.cpp src/Texture.cpp)
find_package(OpenMP REQUIRED)
target_link_libraries(rasterizer SDL2 SDL2main OpenMP::OpenMP_CXX)

target_include_directories(rasterizer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_features(rasterizer PRIVATE cxx_std_20)

target_compile_options(rasterizer PRIVATE 
    -Wall 
    -Wextra 
    -Wpedantic 
    -Werror
)
