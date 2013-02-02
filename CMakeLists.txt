cmake_minimum_required(VERSION 2.8)

project(bluegitf C CXX)

enable_testing()

find_package(GnuTLS REQUIRED)

file(STRINGS VERSION BLUEGITF_VERSION)

message("bluegitf version: ${BLUEGITF_VERSION}")

add_definitions(
  -std=c++0x
  -D_GNU_SOURCE -D_LARGEFILE64_SOURCE
  -Wall -Wextra -Werror=return-type
  ${GNUTLS_DEFINITIONS})

set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -std=c99")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

add_subdirectory(mimosa EXCLUDE_FROM_ALL)
include_directories(mimosa)
add_subdirectory(bluegitf)
