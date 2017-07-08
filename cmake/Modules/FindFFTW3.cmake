# - Try to find fftw3
# Once done this will define
#  FFTW3_FOUND - System has fftw3
#  FFTW3_INCLUDE_DIRS - The fftw3 include directories
#  FFTW3_LIBRARIES - The libraries needed to use fftw3
#  FFTW3_DEFINITIONS - Compiler switches required for using fftw3

find_package(PkgConfig QUIET)
pkg_check_modules(PC_FFTW3 QUIET fftw3)
set(FFTW3_DEFINITIONS ${PC_FFTW3_CFLAGS_OTHER})

find_path(FFTW3_INCLUDE_DIR NAMES fftw3.h
          HINTS ${PC_FFTW3_INCLUDEDIR} ${PC_FFTW3_INCLUDE_DIRS} )

find_library(
  FFTW3_LIBRARY NAMES fftw3
  HINTS ${PC_FFTW3_LIBDIR} ${PC_FFTW3_LIBRARY_DIRS} )

if(PC_FFTW3_VERSION)
  set(FFTW3_VERSION_STRING ${PC_FFTW3_VERSION})
endif()

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set FFTW3_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(
  FFTW3
  REQUIRED_VARS FFTW3_LIBRARY FFTW3_INCLUDE_DIR
  VERSION_VAR FFTW3_VERSION_STRING)

mark_as_advanced(FFTW3_INCLUDE_DIR FFTW3_LIBRARY )

set(FFTW3_LIBRARIES ${FFTW3_LIBRARY} )
set(FFTW3_INCLUDE_DIRS ${FFTW3_INCLUDE_DIR} )
