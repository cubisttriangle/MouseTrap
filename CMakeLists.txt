cmake_minimum_required( VERSION 3.10 )

# This project's name.
project( MouseTrap )

# CMake behavioral flags.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(BUILD_SHARED_LIBS OFF)
set(BUILD_TESTING OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Compiler flags.
set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic" )

# Some convenience variables.
set( SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Src" )
set( BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}" )
set( BIN_SRC_DIR "${BIN_DIR}/src" )

# Add the binary tree to the search path for include files
# so that we will find MouseTrapConfig.h.
include_directories( "${BIN_DIR}" "${SRC_DIR}" )

# Add MouseTrap executable and the sources it depends on.
add_executable( main
  "${SRC_DIR}/main.cpp"
  "${SRC_DIR}/Socket.cpp"
  "${SRC_DIR}/Utils.cpp"
  "${SRC_DIR}/zeroconf.hpp"
  "${SRC_DIR}/zeroconf-util.hpp"
  "${SRC_DIR}/zeroconf-detail.hpp"
)
