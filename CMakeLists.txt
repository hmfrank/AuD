cmake_minimum_required(VERSION 3.5)
project(AuD)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES tst/main.cpp lib/catch.hpp)
add_executable(AuD ${SOURCE_FILES})