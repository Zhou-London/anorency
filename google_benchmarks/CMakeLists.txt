include(FetchContent)

FetchContent_Declare(
    benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.8.3
)

set(BENCHMARK_ENABLE_TESTING OFF)

FetchContent_MakeAvailable(benchmark)

add_executable(ano_benchmarks
    benchmarks.cpp
)

find_package(Threads REQUIRED)

target_link_libraries(ano_benchmarks
    PRIVATE
    anorency
    benchmark::benchmark
    Threads::Threads
)
