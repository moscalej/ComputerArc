cmake_minimum_required(VERSION 3.7)
project(Hw3)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES dflow_calc.cpp dflow_calc.h dflow_main.c)
add_executable(HW3 ${SOURCE_FILES})