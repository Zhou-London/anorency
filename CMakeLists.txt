cmake_minimum_required(VERSION 3.31)

project(anorency)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(anorency
    src/All.cpp
    src/version.cpp
)

target_include_directories(anorency PUBLIC
    include
    include/Types
    include/Streams
    include/Messages
    include/Pools
    include/interfaces
)

target_link_libraries(anorency PUBLIC
    pthread
)

enable_testing()

add_subdirectory(google_tests)
add_subdirectory(google_benchmarks)
