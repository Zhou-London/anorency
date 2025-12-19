cmake_minimum_required(VERSION 3.31)

project(anorency)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(anorency 
    src/info/version.cpp)

target_include_directories(anorency PUBLIC
    include/info
)

enable_testing()

add_subdirectory(tests)
