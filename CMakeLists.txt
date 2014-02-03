cmake_minimum_required(VERSION 2.8)

project(scissy C CXX)

enable_testing()

find_package(GnuTLS REQUIRED)

file(STRINGS VERSION SCISSY_VERSION)

message("scissy version: ${SCISSY_VERSION}")
message("PREFIX: ${CMAKE_INSTALL_PREFIX}")

add_definitions(
  -DINSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
  -std=c++0x
  -D_GNU_SOURCE -D_LARGEFILE64_SOURCE
  -Wall -Wextra -Werror=return-type
  -DSCISSY_VERSION="${SCISSY_VERSION}"
  ${GNUTLS_DEFINITIONS})

set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -std=c99")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

add_subdirectory(mimosa EXCLUDE_FROM_ALL)
include_directories(mimosa)

include(CheckIncludeFiles)
include(CheckLibraryExists)

add_subdirectory(scissy)

install(
  DIRECTORY www/
  DESTINATION share/scissy/www
  PATTERN *.less EXCLUDE
  PATTERN update.sh EXCLUDE)

install(
  DIRECTORY sql/
  DESTINATION share/scissy/sql)

install(FILES
  scripts/scissy-init
  /usr/bin/scissy-upgrade
  scripts/scissy-set-admin
  DESTINATION bin
  PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE
              GROUP_READ GROUP_EXECUTE
              WORLD_READ WORLD_EXECUTE)
install(FILES config/config.json DESTINATION /etc/scissy/)
