# - Try to find blitz++
# Once done this will define
#  BLITZ_FOUND - System has blitz++
#  BLITZ_INCLUDE_DIRS - The blitz++ include directories
#  BLITZ_LIBRARIES - The libraries needed to use blitz++
#  BLITZ_DEFINITIONS - Compiler switches required for using blitz++

find_package(PkgConfig QUIET)
pkg_check_modules(PC_BLITZ QUIET blitz)
set(BLITZ_DEFINITIONS ${PC_BLITZ_CFLAGS_OTHER})

find_path(BLITZ_INCLUDE_DIR NAMES blitz/blitz.h
          HINTS ${PC_BLITZ_INCLUDEDIR} ${PC_BLITZ_INCLUDE_DIRS}
          PATH_SUFFIXES blitz )

find_library(
  BLITZ_LIBRARY NAMES blitz
  HINTS ${PC_BLITZ_LIBDIR} ${PC_BLITZ_LIBRARY_DIRS} )

if(PC_BLITZ_VERSION)
  set(BLITZ_VERSION_STRING ${PC_BLITZ_VERSION})
endif()

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set BLITZ_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(
  Blitz
  REQUIRED_VARS BLITZ_LIBRARY BLITZ_INCLUDE_DIR
  VERSION_VAR BLITZ_VERSION_STRING)

mark_as_advanced(BLITZ_INCLUDE_DIR BLITZ_LIBRARY )

set(BLITZ_LIBRARIES ${BLITZ_LIBRARY} )
set(BLITZ_INCLUDE_DIRS ${BLITZ_INCLUDE_DIR} )
