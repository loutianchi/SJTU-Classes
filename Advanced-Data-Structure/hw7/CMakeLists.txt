cmake_minimum_required(VERSION 3.24)
project(hw7)

set(CMAKE_CXX_STANDARD 17)

add_executable(hw7 main.cpp select.cpp select.h correctness.cpp correctness.h ycsb_test.cpp ycsb_test.h)
