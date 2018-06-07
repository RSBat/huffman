cmake_minimum_required(VERSION 3.9)
project(huffman)

set(CMAKE_CXX_STANDARD 17)
include_directories(${huffman_SOURCE_DIR})

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -D_GLIBCXX_DEBUG")
endif()

add_executable(huffman main.cpp library/huffman.cpp library/huffman.h library/bitstream.cpp library/bitstream.h)

add_executable(huffman_testing
        tests/huffman_testing.cpp
        library/huffman.h
        library/huffman.cpp
        library/bitstream.h
        library/bitstream.cpp
        gtest/gtest-all.cc
        gtest/gtest.h
        gtest/gtest_main.cc )

target_link_libraries(huffman_testing -lpthread)