add_executable(bluegitf
  main.cc

  # web logic and common stuff
  config.cc
  db.cc
  groups.cc
  load-tpl.cc
  page-footer.cc
  page-header.cc
  repositories.cc
  role.cc
  session.cc

  # handlers
  clear-cache-handler.cc
  groups-handler.cc
  login-handler.cc
  logout-handler.cc
  new-group-handler.cc
  new-repo-handler.cc
  register-handler.cc
  repos-handler.cc
  root-handler.cc
  settings-handler.cc
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
