cmake_minimum_required(VERSION 3.28)
project(solaris CXX)

include(cmake/dependencies.cmake)

add_subdirectory(solaris)
add_subdirectory(test)
add_subdirectory(application)
