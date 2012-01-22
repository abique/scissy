protoc_mimosa(acl)

add_library(bluegitf-pb
  acl.pb.cc
  )

add_executable(bluegitf-shell
  shell.cc)

target_link_libraries(bluegitf-shell
  bluegitf-pb
  mimosa-rpc
  mimosa
  protobuf
  gnutls
  pthread
  rt)

add_executable(bluegitfd
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
  group-detail-handler.cc
  login-handler.cc
  logout-handler.cc
  new-group-handler.cc
  new-repo-handler.cc
  register-handler.cc
  repos-handler.cc
  root-handler.cc
  settings-handler.cc

  # rpc
)

target_link_libraries(bluegitfd
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
