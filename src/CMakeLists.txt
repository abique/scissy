add_subdirectory(libgit2)
add_subdirectory(mimosa)

include_directories(mimosa)

add_executable(bluegitfd
  bluegitfd.cc)

target_link_libraries(bluegitfd gflags)
