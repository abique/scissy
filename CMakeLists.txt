cmake_minimum_required(VERSION 2.8)

project(bluegitf C CXX)

enable_testing()

add_definitions(-D_GNU_SOURCE -D_LARGEFILE64_SOURCE -Wall -Wextra)

set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -std=c99")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

add_subdirectory(bluegitf)
