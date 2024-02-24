cmake_minimum_required(VERSION 3.28)
project(solaris CXX)

add_library(solaris source/solaris.cpp
        include/solaris/core/bus.hpp
        include/solaris/core/dispatcher.hpp
        include/solaris/core/layer.hpp
        include/solaris/core/layer_stack.hpp
        include/solaris/core/queue.hpp)
target_include_directories(solaris PUBLIC include)

add_subdirectory(application)
