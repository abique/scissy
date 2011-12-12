add_executable(bluegitf-web-server
  main.cc

  # logic
  load-tpl.cc
  session.cc
  page-header.cc

  # handlers
  clear-cache-handler.cc
  root-handler.cc

  )

target_link_libraries(bluegitf-web-server
  bluegitf-db-proto
  bluegitf)
