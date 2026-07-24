cmake_minimum_required(VERSION 3.20)

project(raytracer VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL3 CONFIG REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

function(configure_raytracer_target target_name)
    target_compile_features(${target_name} PRIVATE cxx_std_20)

    if(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang|Clang|GNU")
        target_compile_options(
            ${target_name}
            PRIVATE
                -Wall
                -Wextra
                -Wpedantic
        )
    endif()
endfunction()

add_executable(
    raytracer
    src/main.cpp
    src/Vector.cpp
    src/Camera.cpp
    src/Image.cpp 
    src/LightSource.cpp
    src/Material.cpp
    src/Ray.cpp 
    src/RayTracer.cpp 
    src/Scene.cpp 
    src/Shape.cpp 
)

target_include_directories(
    raytracer
    PRIVATE
        "${PROJECT_SOURCE_DIR}/include"
)

target_link_libraries(raytracer PRIVATE SDL3::SDL3)
configure_raytracer_target(raytracer)

add_executable(
    sdl_example
    examples/example.cpp
)

target_link_libraries(sdl_example PRIVATE SDL3::SDL3)
configure_raytracer_target(sdl_example)
