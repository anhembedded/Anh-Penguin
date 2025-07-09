cmake_minimum_required(VERSION 3.15)

project(PRACTICE_LINUX VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_subdirectory(src_1)
add_subdirectory(src_2)
add_subdirectory(src_3)
add_subdirectory(src_4)
add_subdirectory(runProcess)
