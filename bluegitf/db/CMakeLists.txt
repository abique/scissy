protoc_mimosa(service)

add_library(bluegitf-db-proto
  service.pb.cc)

add_executable(bluegitf-db
  main.cc)

target_link_libraries(bluegitf-db
  bluegitf-db-proto
  mimosa
  mimosa-options
  gnutls
  protobuf)
