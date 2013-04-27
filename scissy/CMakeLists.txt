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
  repositories.cc
  role.cc
  session.cc

  # handlers
  root-handler.cc

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
