cmake_minimum_required(VERSION 3.16)

# used as a module of other projects
if(NOT DEFINED PROJECT_NAME)
    project(core LANGUAGES CXX)
    add_compile_options(-Wall -Wextra -Werror)
endif()

# options
set(libcore_FMT_PROVIDER
    "package"
    CACHE STRING "Provider of fmt library")
set_property(CACHE libcore_FMT_PROVIDER PROPERTY STRINGS "module" "package")

set(libcore_LIBEVENT_PROVIDER
    "package"
    CACHE STRING "Provider of LIBEVENT library")
set_property(CACHE libcore_LIBEVENT_PROVIDER PROPERTY STRINGS "module" "package")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(ZLIB REQUIRED)

# libcgroup support pkg-config, but not cmake
find_package(PkgConfig)
pkg_check_modules(LIBCGROUP REQUIRED libcgroup)

if(libcore_FMT_PROVIDER STREQUAL "module")
    add_subdirectory(third_party/fmt)
else()
    find_package(fmt CONFIG REQUIRED)
endif()

if(libcore_LIBEVENT_PROVIDER STREQUAL "module")
    set(EVENT__DISABLE_TESTS ON)
    add_subdirectory(third_party/libevent)
else()
    find_package(Libevent CONFIG REQUIRED)
endif()

include_directories(core/include)
add_subdirectory(core)
add_library(core INTERFACE)
target_include_directories(core INTERFACE "core/include")
target_link_libraries(core INTERFACE libcore::common libcore::component libcore::event libcore::process libcore::http
                                     libcore::os)
