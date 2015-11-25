cmake_minimum_required(VERSION 2.6)

project(luacxx)
set(PROJECT_DESCRIPTION "C++ Lua binding (c++11 / c++14)")
set(PROJECT_VERSION 0.0.1 CACHE STRING "Version number")

set(LUACXX_TESTS_DIR        "${CMAKE_SOURCE_DIR}/tests/")
set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -Wall -Wextra -ggdb -std=c++1y -I/home/luther/dev/github/root/include")
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}   -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_MODULE_PATH 			 ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

if( NOT CMAKE_BUILD_TYPE )
	set( CMAKE_BUILD_TYPE Debug CACHE STRING  "Choose the type of build, options are: Debug Release" FORCE )
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

include_directories(
		include
		${CMAKE_CURRENT_BINARY_DIR}
)

#
# Lua
find_package(
	Lua52
)

include_directories(
	${LUA_INCLUDE_DIR}
)

if(LUA_INCLUDE_DIR)
	message(STATUS "Lua52: Found")
else ()
	message(SEND_ERROR "Could not find Lua52")
endif ()

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
add_subdirectory(src)
add_subdirectory(tests)

#
# configure file
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/luacxx.pc.in ${CMAKE_CURRENT_BINARY_DIR}/luacxx.pc @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)

#
# install
install(DIRECTORY include/ DESTINATION include)
install(FILES "${CMAKE_BINARY_DIR}/toolsbox.pc" DESTINATION lib/pkgconfig)
