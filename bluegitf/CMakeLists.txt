add_executable(bluegitf
  main.cc

  # web logic and common stuff
  config.cc
  db.cc
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
  mimosa-tpl
  mimosa-http
  mimosa-sqlite
  mimosa
  gnutls
  protobuf
  sqlite3
  pthread
  rt
)
