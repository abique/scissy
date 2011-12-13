add_subdirectory(db)
add_subdirectory(web-server)

add_executable(bluegitf
  main.cc
  db.cc

  # web logic and common stuff
  config.cc
  load-tpl.cc
  page-footer.cc
  page-header.cc
  session.cc

  # handlers
  clear-cache-handler.cc
  register-handler.cc
  root-handler.cc
)

target_link_libraries(bluegitf
  mimosa
  gnutls
  protobuf
  pthread
  rt
)
