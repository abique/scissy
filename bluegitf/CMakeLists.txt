add_executable(bluegitf
  main.cc

  # web logic and common stuff
  config.cc
  db.cc
  load-tpl.cc
  page-footer.cc
  page-header.cc
  repositories.cc
  session.cc

  # handlers
  clear-cache-handler.cc
  login-handler.cc
  register-handler.cc
  root-handler.cc
  new-repo-handler.cc
)

target_link_libraries(bluegitf
  mimosa-tpl
  mimosa-http
  mimosa-sqlite
  mimosa
  gnutls
  protobuf
  sqlite3
  crack
  git2
  pthread
  rt
)
