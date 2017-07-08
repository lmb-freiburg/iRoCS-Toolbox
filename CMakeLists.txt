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

# ===============================
# User options
# ===============================
option(BUILD_STATIC_LIBS "Build static libraries" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_TESTS "Build tests" OFF)
option(VERBOSE_DEBUG "Enable verbose debugging output" OFF)
if(VERBOSE_DEBUG)
  set(DEBUG "1")
endif()
option(BLITZ_DEBUG
  "Enable Blitz++ debugging (e.g. range checking) (very slow!)" OFF)
if(BLITZ_DEBUG)
  set(BZ_DEBUG "1")
endif()

# ==================================
# Set warn level
# ==================================
if(MSVC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W2")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W2")
endif()

if (CMAKE_COMPILER_IS_GNUCC)
  set(CMAKE_C_FLAGS
    "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic -Wno-long-long")
endif()

if (CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wno-long-long")
  # Check for undefined symbols when linking shared libraries to get early
  # feedback on missing dependencies
  set(CMAKE_SHARED_LINKER_FLAGS
    "-Wl,--no-undefined ${CMAKE_SHARED_LINKER_FLAGS}" )
endif()

# ===============================
# Search for external packages
# ===============================
find_package(OpenMP)
find_package(Blitz 0.10 REQUIRED)

# If found, turn OpenMP on for all submodules and set BZ_THREADSAFE
if(OPENMP_FOUND)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
  set(BZ_THREADSAFE "1")
endif()

find_package(HDF5 1.8 REQUIRED)
find_package(HDF5_CPP 1.8 REQUIRED)
find_package(PNG REQUIRED)
find_package(TIFF REQUIRED)
find_package(JPEG REQUIRED)
find_package(FFTW3 REQUIRED)
find_package(FFTW3F REQUIRED)
find_package(GSL REQUIRED)
find_package(OpenCV REQUIRED COMPONENTS core imgproc)
find_package(Qt4 4.6 REQUIRED COMPONENTS QtCore QtGui QtOpenGL QtXml QtSvg )
find_package(OpenGL REQUIRED)
find_package(Qt5Widgets QUIET)

# ===============================
# Write configuration header
# ===============================
configure_file(
  "${PROJECT_SOURCE_DIR}/src/config-cmake.hh.in"
  "${PROJECT_BINARY_DIR}/src/config.hh" )
add_definitions(-DHAVE_CONFIG_H)
include_directories(
  "${PROJECT_SOURCE_DIR}/src"
  "${PROJECT_BINARY_DIR}/src" )

# ===============================
# Recurse into subdirectories
# ===============================

add_subdirectory(src)

# =================================
# If user wants tests, build them
# =================================
if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif(BUILD_TESTS)
