cmake_minimum_required(VERSION 3.14)
project(diversityhack)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(diversityhack
        data_types.h
        MCT.cpp
        MCT.h
        parse.cpp)
