cmake_minimum_required(VERSION 3.28)
project(solaris CXX)

add_library(solaris source/solaris.cpp)
target_include_directories(solaris PUBLIC include)

add_subdirectory(application)
