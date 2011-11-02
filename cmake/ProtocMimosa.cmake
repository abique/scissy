macro(protoc_mimosa name)
  add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${name}.pb.h ${CMAKE_CURRENT_SOURCE_DIR}/${name}.pb.cc
    COMMAND protoc --mimosa_out=. ${name}.proto
    DEPENDS ${name}.proto
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  set_source_files_properties(
    ${CMAKE_CURRENT_SOURCE_DIR}/${name}.pb.h
    ${CMAKE_CURRENT_SOURCE_DIR}/${name}.pb.cc
    PROPERTIES GENERATED true)
endmacro(protoc_mimosa)
