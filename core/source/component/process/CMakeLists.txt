add_library(process manager.cc process.cc)
add_library(libcore::process ALIAS process)
target_link_libraries(process PRIVATE fmt::fmt)
