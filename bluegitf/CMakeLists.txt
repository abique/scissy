add_subdirectory(db)
add_subdirectory(web-server)

add_library(bluegitf
  config.cc)

target_link_libraries(bluegitf mimosa mimosa-options)
