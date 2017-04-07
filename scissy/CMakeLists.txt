mimosa_protoc(service)
mimosa_protoc(config)

add_library(scissy-common SHARED
  service.pb.cc
  log.cc
  ssh-key-type.cc
  )

add_executable(scissy-shell
  shell.cc)

target_link_libraries(scissy-shell
  scissy-common
  mimosa-rpc
  mimosa-rpc-json
  mimosa-http
  mimosa
  protobuf
  gnutls
  pthread
  rt)

add_executable(scissyd
  main.cc
  config.pb.cc
  helpers.cc
  access.cc
  builtin-authenticator.cc
  process-authenticator.cc

  # web logic and common stuff
  config.cc
  db.cc
  groups.cc
  repositories.cc
  role.cc
  gen-authorized-keys.cc

  # handlers
  archive-handler.cc
  logout-handler.cc
  root-handler.cc
  session.cc
  session-handler.cc

  # rpc
  service.cc
)

target_link_libraries(scissyd
  scissy-common
  mimosa-rpc-json
  mimosa-rpc
  mimosa-tpl
  mimosa-http
  mimosa-fs
  mimosa-sqlite
  mimosa-git
  mimosa-archive
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

install(TARGETS scissy-common scissyd scissy-shell
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
