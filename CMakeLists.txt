cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

project(WolfSoundDspUtils VERSION 0.1.1)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
endif()

add_subdirectory(src)

option(BUILD_TESTS "Enable test targets of this library" OFF)
if(BUILD_TESTS)
  enable_testing()
  set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs")
  include(cmake/cpm.cmake)
  add_subdirectory(tests)
endif()
