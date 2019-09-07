cmake_minimum_required(VERSION 3.14)
project(Dostavista)

set(CMAKE_CXX_STANDARD 17)

add_executable(Dostavista parse.cpp data_types.h MCT.cpp MCT.h)