# - Try to find hdf5 C++ interface
# Once done this will define
#  HDF5_CPP_FOUND - System has hdf5_cpp
#  HDF5_CPP_INCLUDE_DIRS - The hdf5_cpp include directories
#  HDF5_CPP_LIBRARIES - The libraries needed to use hdf5_cpp
#  HDF5_CPP_DEFINITIONS - Compiler switches required for using hdf5_cpp

find_package(PkgConfig QUIET)
pkg_check_modules(PC_HDF5 QUIET hdf5)
pkg_check_modules(PC_HDF5_CPP QUIET hdf5_cpp)
if(PC_HDF5_CPP_CFLAGS_OTHER)
  set(HDF5_CPP_DEFINITIONS ${PC_HDF5_CPP_CFLAGS_OTHER})
else()
  set(HDF5_CPP_DEFINITIONS ${PC_HDF5_CFLAGS_OTHER})
endif()

find_path(HDF5_CPP_INCLUDE_DIR NAMES H5Cpp.h
  HINTS ${PC_HDF5_CPP_INCLUDEDIR} ${PC_HDF5_CPP_INCLUDE_DIRS}
  ${PC_HDF5_INCLUDEDIR} ${PC_HDF5_INCLUDE_DIRS} )

find_library(
  HDF5_CPP_LIBRARY NAMES hdf5_cpp
  HINTS ${PC_HDF5_CPP_LIBDIR} ${PC_HDF5_CPP_LIBRARY_DIRS}
  ${PC_HDF5_LIBDIR} ${PC_HDF5_LIBRARY_DIRS} )

if(PC_HDF5_CPP_VERSION)
  set(HDF5_CPP_VERSION_STRING ${PC_HDF5_CPP_VERSION})
elseif(PC_HDF5_VERSION)
  set(HDF5_CPP_VERSION_STRING ${PC_HDF5_VERSION})
endif()

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set HDF5_CPP_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(
  HDF5_CPP
  REQUIRED_VARS HDF5_CPP_LIBRARY HDF5_CPP_INCLUDE_DIR
  VERSION_VAR HDF5_CPP_VERSION_STRING)

mark_as_advanced(HDF5_CPP_INCLUDE_DIR HDF5_CPP_LIBRARY )

set(HDF5_CPP_LIBRARIES ${HDF5_CPP_LIBRARY} )
set(HDF5_CPP_INCLUDE_DIRS ${HDF5_CPP_INCLUDE_DIR} )
