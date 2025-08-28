cmake_minimum_required(VERSION 3.31)

project(anorency)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(anorency src/main.cpp)

target_include_directories(anorency PRIVATE
    src
    src/AnoEnvironment
)

target_sources(anorency PRIVATE
    src/AnoEnvironment/AnoEnvironment.cpp
)