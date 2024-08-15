add_subdirectory(process)

add_library(component thread_container.cc thread_logger.cc)
add_library(libcore::component ALIAS component)

target_link_libraries(component PRIVATE fmt::fmt libcore::event)
