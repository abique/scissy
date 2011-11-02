add_subdirectory(db)
add_subdirectory(web-server)

add_executable(bluegitfd
  bluegitfd.cc)

target_link_libraries(bluegitfd mimosa)
