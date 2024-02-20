cmake_minimum_required(VERSION 3.16)

set(APP_NAME core)

project(${APP_NAME})

set(CMAKE_CXX_STANDARD 17)

# protobuf
file(GLOB_RECURSE CORE_SRCS "core/*.cc")
file(GLOB_RECURSE CORE_C_SRCS "src/*.c")

find_package(ZLIB REQUIRED)

include_directories("core/include")

message(STATUS "${CORE_SRCS}")

add_library(core
        ${CORE_SRCS}
        ${CORE_C_SRCS}
        )
target_link_libraries(core
        event fmt ${ZLIB_LIBRARIES} libevent_pthreads.a libcgroup.a)
