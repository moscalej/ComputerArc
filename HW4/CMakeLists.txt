cmake_minimum_required(VERSION 3.7)
project(HW4)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES cacheSim.cpp Ways.cpp Ways.h Cache.cpp Cache.h DRam.cpp DRam.h)
add_executable(HW4 ${SOURCE_FILES})