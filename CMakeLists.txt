cmake_minimum_required(VERSION 3.31)
project(EmkyLog)

set(CMAKE_CXX_STANDARD 23)



add_executable(EmkyLog main.cpp
        EmkyLog.h)

target_link_options(EmkyLog PRIVATE -static-libgcc -static-libstdc++)