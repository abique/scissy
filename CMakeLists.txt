cmake_minimum_required(VERSION 2.8)

project(scissy C CXX)

enable_testing()

find_package(GnuTLS REQUIRED)

file(STRINGS VERSION SCISSY_VERSION)

set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

message("scissy version: ${SCISSY_VERSION}")
message("PREFIX: ${CMAKE_INSTALL_PREFIX}")

add_definitions(
  -DINSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
  -D_GNU_SOURCE -D_LARGEFILE64_SOURCE
  -Wall -Wextra -Werror=return-type
  -DSCISSY_VERSION="${SCISSY_VERSION}"
  ${GNUTLS_DEFINITIONS})

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

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
  scripts/scissy-upgrade
  scripts/scissy-set-admin
  scripts/scissy-auth-ldap
  DESTINATION lib/scissy/
  PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE
              GROUP_READ GROUP_EXECUTE
              WORLD_READ WORLD_EXECUTE)
install(FILES config/config.json DESTINATION /etc/scissy/)
