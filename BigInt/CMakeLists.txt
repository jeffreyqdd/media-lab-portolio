# Specify the minimum required version of CMake
cmake_minimum_required(VERSION 3.20)

# Define the project name and specify the C++ standard
project(bignum LANGUAGES CXX)

# Set the C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -DDEBUG -ftrapv -fsanitize=undefined -pg")
    message(STATUS "Building in Debug mode")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
    message(STATUS "Building in Release mode")
    set(BUILD_BENCHMARK "YES")
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O2 -DDEBUGOPTIMIZED")
    message(STATUS "Building in DebugOptimized mode")
else()
    message(WARNING "Unknown build type. Defaulting to Release mode.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
    set(CMAKE_BUILD_TYPE "Release")
endif()

add_compile_options(-march=native -flto=auto -Wpedantic -Wall)

add_library(hw4_lib STATIC
    lib/bignum.cpp
    lib/thread_pool.cpp
)

target_include_directories(hw4_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_executable(bignum_old src/part1_main.cpp)
target_link_libraries(bignum_old PRIVATE hw4_lib)

target_include_directories(hw4_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_executable(bignum src/part2_main.cpp)
target_link_libraries(bignum PRIVATE hw4_lib)

target_include_directories(hw4_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE hw4_lib)

# benchmarks
if(BUILD_BENCHMARK STREQUAL "YES")
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable Benchmark testing" FORCE)
    add_subdirectory(dependencies/benchmark)
    add_executable(bench_primitives benchmarks/bench_primitives.cpp)
    target_link_libraries(bench_primitives PRIVATE benchmark::benchmark pthread)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BUILD_GTEST OFF CACHE BOOL "Disable GoogleTest build" FORCE)
    add_subdirectory(dependencies/googletest)
    # tests
    # Add tests to CMake
    enable_testing()
    include(GoogleTest)
    list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")

    function(add_gtest_executable test_source)
        get_filename_component(test_name ${test_source} NAME_WE)
        add_executable(${test_name} ${test_source})
        target_link_libraries(${test_name} PRIVATE hw4_lib gtest_main)
        gtest_discover_tests(${test_name})
    endfunction()

    file(GLOB TEST_SOURCES "${CMAKE_SOURCE_DIR}/tests/*.cpp")
    foreach(test_file ${TEST_SOURCES})
        message("adding test ${test_file}")
        add_gtest_executable(${test_file})
    endforeach()
endif()
