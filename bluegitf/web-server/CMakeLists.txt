add_executable(bluegitf-web-server
  main.cc

  # logic
  session.cc

  # handlers
  clear-cache-handler.cc
  root-handler.cc

  )

target_link_libraries(bluegitf-web-server
  bluegitf-db-proto
  bluegitf
  mimosa-rpc
  mimosa-http
  mimosa-uri
  mimosa-net
  mimosa-stream
  mimosa-runtime
  mimosa-log
  mimosa
  mimosa-options
  gnutls
  protobuf
  ctemplate)
