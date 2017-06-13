cmake_minimum_required(VERSION 3.7)
project(HW4)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES cacheSim.cpp Way.cpp Way.h Cache.cpp Cache.h DRam.cpp DRam.h HelpFunc.h Cache_Master.cpp Cache_Master.h CacheSlave.cpp CacheSlave.h)
add_executable(HW4 ${SOURCE_FILES})