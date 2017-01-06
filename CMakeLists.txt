cmake_minimum_required(VERSION 3.5)
project(AuD)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES tst/main.cpp lib/catch.hpp src/AvlTree.h src/AvlTree.c tst/AvlTree.cpp src/HyperLogLog.c src/HyperLogLog.h)
add_executable(AuD ${SOURCE_FILES})