cmake_minimum_required(VERSION 3.28)
project(solaris CXX C)

include(cmake/dependencies.cmake)

add_subdirectory(solaris)
add_subdirectory(test)
add_subdirectory(application)
add_subdirectory(game)
