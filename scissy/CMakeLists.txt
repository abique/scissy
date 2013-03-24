mimosa_protoc(service)

add_library(scissy-common
  service.pb.cc
  log.cc
  )

# add_executable(scissy-shell
#   shell.cc)

# target_link_libraries(scissy-shell
#   scissy-common
#   mimosa-rpc
#   mimosa
#   protobuf
#   gnutls
#   pthread
#   rt)

add_executable(scissyd
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
  login-handler.cc
  logout-handler.cc
  new-repo-handler.cc
  register-handler.cc
  repos-handler.cc
  root-handler.cc
  settings-handler.cc

  # rpc
  service.cc
)

target_link_libraries(scissyd
  scissy-common
  mimosa-rpc-json
  mimosa-rpc
  mimosa-tpl
  mimosa-http
  mimosa-sqlite
  mimosa
  gnutls
  protobuf
  sqlite3
  crack
  git2
  re2
  pthread
  rt
)
