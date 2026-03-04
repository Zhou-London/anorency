cmake_minimum_required(VERSION 3.31)

project(anon)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(anon
    src/All.cpp
    src/version.cpp
)

target_include_directories(anon PUBLIC
    include
)

find_package(Threads REQUIRED)

target_link_libraries(anon PUBLIC
    Threads::Threads
)

enable_testing()

add_subdirectory(google_tests)
add_subdirectory(google_benchmarks)
