cmake_minimum_required(VERSION 3.16)

set(APP_NAME core)

project(${APP_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

file(GLOB_RECURSE CORE_SRCS "core/*.cc")
file(GLOB_RECURSE CORE_C_SRCS "src/*.c")

find_package(ZLIB REQUIRED)

set(libcore_FMT_PROVIDER
    "package"
    CACHE STRING "Provider of fmt library")
set_property(CACHE libcore_FMT_PROVIDER PROPERTY STRINGS "module" "package")

set(libcore_LIBEVENT_PROVIDER
    "package"
    CACHE STRING "Provider of LIBEVENT library")
set_property(CACHE libcore_LIBEVENT_PROVIDER PROPERTY STRINGS "module" "package")

if(libcore_FMT_PROVIDER STREQUAL "module")
    add_subdirectory(third_party/fmt)
else()
    find_package(fmt REQUIRED)
endif()

if(libcore_LIBEVENT_PROVIDER STREQUAL "module")
    set(EVENT__DISABLE_TESTS ON)
    add_subdirectory(third_party/libevent)
else()
    find_package(Libevent CONFIG REQUIRED)
endif()

include_directories("core/include")

message(STATUS "core files ${CORE_SRCS}")

add_library(core ${CORE_SRCS} ${CORE_C_SRCS})

target_link_libraries(core fmt::fmt ${ZLIB_LIBRARIES} libcgroup.a)
if(libcore_LIBEVENT_PROVIDER STREQUAL "module")
    target_link_libraries(core event_core event_pthreads)
else()
    target_link_libraries(core libevent::core libevent::pthreads)
endif()
