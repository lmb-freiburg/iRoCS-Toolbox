cmake_minimum_required(VERSION 2.8.11)

project(iRoCSToolbox)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set(PACKAGE "irocs-toolbox")
set(PACKAGE_NAME "iRoCS Toolbox")
set(PACKAGE_BUGREPORT "falk@cs.uni-freiburg.de")
set(PACKAGE_STRING "iRoCS Toolbox 1.1.2")
set(PACKAGE_TARNAME "irocs-toolbox")
set(PACKAGE_URL "http://lmb.informatik.uni-freiburg.de/lmbsoft/iRoCS")
set(PACKAGE_VERSION "1.1.2")
set(VERSION "1.1.2")

option(BUILD_STATIC_LIBS "Build static libraries" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_TESTS "Build tests" OFF)

find_package(OpenMP)

find_package(Blitz 0.10 REQUIRED)
if(OPENMP_FOUND)
  set(BZ_THREADSAFE "1")
endif()
option(BLITZ_DEBUG "Enable Blitz++ debugging (e.g. range checking) (very slow!)" OFF)
if(BLITZ_DEBUG)
  set(BZ_DEBUG "1")
endif()

option(VERBOSE_DEBUG "Enable verbose debugging output" OFF)
if(VERBOSE_DEBUG)
  set(DEBUG "1")
endif()

configure_file(
  "${PROJECT_SOURCE_DIR}/src/config-cmake.hh.in"
  "${PROJECT_BINARY_DIR}/src/config.hh" )

add_definitions(-DHAVE_CONFIG_H)

include_directories(
  "${iRoCSToolbox_SOURCE_DIR}/src"
  "${iRoCSToolbox_BINARY_DIR}/src" )

add_subdirectory(src)

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif(BUILD_TESTS)
