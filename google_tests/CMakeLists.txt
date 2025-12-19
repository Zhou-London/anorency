include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

FetchContent_MakeAvailable(googletest)

add_executable(ano_tests
    tests.cpp
)

target_link_libraries(ano_tests
    PRIVATE
    anorency
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(ano_tests)
