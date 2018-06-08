#!/bin/bash

g++ -O0 -fprofile-arcs -ftest-coverage -lpthread -I./ -o all_tests tests/huffman_testing.cpp gtest/gtest-all.cc gtest/gtest_main.cc library/huffman.cpp library/bitstream.cpp
./all_tests
gcovr -r . --html --html-details -d -o coverage.html
rm *.gcno