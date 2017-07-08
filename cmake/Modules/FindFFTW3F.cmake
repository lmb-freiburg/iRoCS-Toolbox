# - Try to find fftw3f
# Once done this will define
#  FFTW3F_FOUND - System has fftw3f
#  FFTW3F_INCLUDE_DIRS - The fftw3f include directories
#  FFTW3F_LIBRARIES - The libraries needed to use fftw3f
#  FFTW3F_DEFINITIONS - Compiler switches required for using fftw3f

find_package(PkgConfig QUIET)
pkg_check_modules(PC_FFTW3F QUIET fftw3f)
set(FFTW3F_DEFINITIONS ${PC_FFTW3F_CFLAGS_OTHER})

find_path(FFTW3F_INCLUDE_DIR NAMES fftw3.h
          HINTS ${PC_FFTW3F_INCLUDEDIR} ${PC_FFTW3F_INCLUDE_DIRS} )

find_library(
  FFTW3F_LIBRARY NAMES fftw3f
  HINTS ${PC_FFTW3F_LIBDIR} ${PC_FFTW3F_LIBRARY_DIRS} )

if(PC_FFTW3F_VERSION)
  set(FFTW3F_VERSION_STRING ${PC_FFTW3F_VERSION})
endif()

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set FFTW3F_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(
  FFTW3F
  REQUIRED_VARS FFTW3F_LIBRARY FFTW3F_INCLUDE_DIR
  VERSION_VAR FFTW3F_VERSION_STRING)

mark_as_advanced(FFTW3F_INCLUDE_DIR FFTW3F_LIBRARY )

set(FFTW3F_LIBRARIES ${FFTW3F_LIBRARY} )
set(FFTW3F_INCLUDE_DIRS ${FFTW3F_INCLUDE_DIR} )
