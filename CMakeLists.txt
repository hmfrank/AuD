cmake_minimum_required(VERSION 3.5)
project(AuD)

set(CMAKE_CXX_STANDARD 11)

include_directories(${CMAKE_SOURCE_DIR}/lib/)

set(SOURCE_FILES tst/main.cpp lib/catch.hpp src/AvlTree.h src/AvlTree.c tst/AvlTree.cpp src/HyperLogLog.c inc/HyperLogLog.h tst/HyperLogLog.cpp inc/tzcnt.h)
add_executable(AuD ${SOURCE_FILES})

# TODO: fix cmake file