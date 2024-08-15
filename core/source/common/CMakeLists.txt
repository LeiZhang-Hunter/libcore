add_library(common helper.cc)
add_library(libcore::common ALIAS common)

target_include_directories(common PUBLIC ${CMAKE_SOURCE_DIR}/core/include)
target_link_libraries(common PRIVATE ${ZLIB_LIBRARIES})
