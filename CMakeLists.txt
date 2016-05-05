cmake_minimum_required(VERSION 3.0)
project(luacxx VERSION 0.0.1)

set(PROJECT_DESCRIPTION "C++ Lua binding (c++11 / c++14)")

set(LUACXX_TESTS_DIR        "${CMAKE_SOURCE_DIR}/tests/")
set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -Wall -Wextra -ggdb -std=c++1y")
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}   -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_MODULE_PATH       "${CMAKE_SOURCE_DIR}/cmake/")

if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Debug CACHE STRING  "Choose the type of build, options are: Debug Release" FORCE )
endif()

if(NOT DISABLE_UNITTEST)
  set(DISABLE_UNITTEST OFF CACHE BOOL "Disable unittest" FORCE)
endif()

enable_testing()

#
# Boost
include(FindBoost)

find_package(
  Boost
  1.50.0
  COMPONENTS unit_test_framework
)

#
# Lua
find_package(
  Lua53
)

if(LUA_INCLUDE_DIR)
  message(STATUS "Lua53: Found")
else ()
  message(SEND_ERROR "Could not find Lua53")
endif ()

#
# Toolsbox
find_package(PkgConfig)
pkg_check_modules(toolsbox REQUIRED toolsbox)

#
# update
include_directories(
    include
    ${CMAKE_CURRENT_BINARY_DIR}
    ${LUA_INCLUDE_DIR}
    ${toolsbox_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIR}
)

#
# doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    @ONLY
  )
  add_custom_target(
    doc
    ${DOXYGEN_EXECUTABLE}
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY
    ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM
  )
endif(DOXYGEN_FOUND)

#
# subdir
add_subdirectory(tests)

#
# configure file
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/luacxx.pc.in ${CMAKE_CURRENT_BINARY_DIR}/luacxx.pc @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)

#
# install
install(DIRECTORY include/ DESTINATION include)
install(FILES "${CMAKE_BINARY_DIR}/luacxx.pc" DESTINATION lib/pkgconfig)
