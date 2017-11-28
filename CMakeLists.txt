cmake_minimum_required(VERSION 2.8)

project(iRoCSToolbox)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(PACKAGE "irocs-toolbox")
set(PACKAGE_NAME "iRoCS Toolbox")
set(PACKAGE_BUGREPORT "falk@cs.uni-freiburg.de")
set(PACKAGE_STRING "iRoCS Toolbox 1.2.1")
set(PACKAGE_TARNAME "irocs-toolbox")
set(PACKAGE_URL "http://lmb.informatik.uni-freiburg.de/lmbsoft/iRoCS")
set(PACKAGE_VERSION "1.2.1")
set(VERSION "1.2.1")
set(iRoCS-Toolbox_VERSION 1.2.1)

# ===============================
# User options
# ===============================
option(BUILD_STATIC_LIBS "Build static libraries" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_STATIC_TOOLS "Build statically linked tools" OFF)
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
# Set absolute install paths
# ===============================
if (WIN32 AND NOT CYGWIN)
  set(INSTALL_CMAKE_DIR "CMake")
else()
  set(INSTALL_CMAKE_DIR "lib/CMake/iRoCS-Toolbox")
endif()
set(INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include/${PACKAGE}")
set(INSTALL_LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib")
set(INSTALL_BIN_DIR "${CMAKE_INSTALL_PREFIX}/bin")
set(INSTALL_CMAKE_DIR "${CMAKE_INSTALL_PREFIX}/${INSTALL_CMAKE_DIR}")

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

if(UNIX)
  find_library(LIBM_LIBRARIES m DOC "The standard math library")
  if (NOT LIBM_LIBRARIES)
    message(FATAL_ERROR "Standard math library not found")
  endif()
  mark_as_advanced(LIBM_LIBRARIES)
endif()
find_package(ZLIB REQUIRED)
find_package(HDF5 1.8 REQUIRED COMPONENTS C CXX)
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

if(BUILD_STATIC_TOOLS)

  macro(find_static_library shared_lib outvarprefix module)
    get_filename_component(folder ${shared_lib} DIRECTORY)
    get_filename_component(libname ${shared_lib} NAME_WE)
    string(REGEX REPLACE "^lib" "" libname ${libname})
    if ("${module}" STREQUAL "")
      set(outvar ${outvarprefix}_STATIC_LIBRARY)
    else()
      set(outvar ${module}_${outvarprefix}_STATIC_LIBRARY)
    endif()
    find_library(${outvar} ${CMAKE_STATIC_LIBRARY_PREFIX}${libname}${CMAKE_STATIC_LIBRARY_SUFFIX}
      HINTS ${folder} DOC "The static ${libname} library")
    mark_as_advanced(${outvar})
    if (NOT ${outvar})
      message(FATAL_ERROR "Static ${libname} library not found.")
    endif()
    if (NOT "${module}" STREQUAL "")
      list(APPEND ${module}_STATIC_LIBRARIES ${${outvar}})
    else()
      set(${outvarprefix}_STATIC_LIBRARIES ${${outvar}})
    endif()
  endmacro()

  if (UNIX)
    find_library(LIBM_STATIC_LIBRARIES ${CMAKE_STATIC_LIBRARY_PREFIX}m${CMAKE_STATIC_LIBRARY_SUFFIX})
    if (NOT LIBM_STATIC_LIBRARIES)
      message(FATAL_ERROR "Static standard math library not found")
    endif()
    mark_as_advanced(LIBM_STATIC_LIBRARIES)
  endif()
  find_static_library(${ZLIB_LIBRARY} "ZLIB" "")

  set(THIRDPARTY_PREFIX "thirdparty")

  include(ExternalProject)

  # Build our own HDF5, because the Ubuntu version links against
  # the wrong libsz :(
  if (WIN32)
    set(HDF5_URL "https://support.hdfgroup.org/ftp/HDF5/current18/src/hdf5-1.8.19.zip")
    set(HDF5_URL_MD5 "9cc798bb14160db01c25324f33941dce")
  else()
    set(HDF5_URL "https://support.hdfgroup.org/ftp/HDF5/current18/src/hdf5-1.8.19.tar.gz")
    set(HDF5_URL_MD5 "7f568e2464d4ab0a74d16b23956d900b")
  endif()
  list(APPEND HDF5_CMAKE_ARGS "-DBUILD_SHARED_LIBS:BOOL=OFF")
  list(APPEND HDF5_CMAKE_ARGS "-DCMAKE_BUILD_TYPE:string=Release")
  list(APPEND HDF5_CMAKE_ARGS "-DHDF5_BUILD_CPP_LIB:BOOL=ON")
  list(APPEND HDF5_CMAKE_ARGS "-DHDF5_BUILD_EXAMPLES:BOOL=OFF")
  list(APPEND HDF5_CMAKE_ARGS "-DHDF5_BUILD_FORTRAN:BOOL=OFF")
  list(APPEND HDF5_CMAKE_ARGS "-DHDF5_BUILD_HL_LIB:BOOL=OFF")
  list(APPEND HDF5_CMAKE_ARGS "-DHDF5_BUILD_TOOLS:BOOL=OFF")
  list(APPEND HDF5_CMAKE_ARGS "-DHDF_ENABLE_LARGE_FILE:BOOL=ON")
  list(APPEND HDF5_CMAKE_ARGS "-DHDF5_ENABLE_Z_LIB_SUPPORT:BOOL=ON")
  list(APPEND HDF5_CMAKE_ARGS "-DHDF5_ENABLE_SZIP_SUPPORT:BOOL=OFF")
  ExternalProject_Add(
    hdf5_static
    PREFIX ${THIRDPARTY_PREFIX}
    URL ${HDF5_URL}
    URL_MD5 ${HDF5_URL_MD5}
    CMAKE_ARGS ${HDF5_CMAKE_ARGS}
    INSTALL_COMMAND "")
  list(APPEND HDF5_STATIC_C_INCLUDE_DIRS
    ${PROJECT_BINARY_DIR}/${THIRDPARTY_PREFIX}/src/hdf5_static/src)
  list(APPEND HDF5_STATIC_C_INCLUDE_DIRS
    ${PROJECT_BINARY_DIR}/${THIRDPARTY_PREFIX}/src/hdf5_static-build)
  set(HDF5_STATIC_CXX_INCLUDE_DIRS ${HDF5_STATIC_C_INCLUDE_DIRS})
  list(APPEND HDF5_STATIC_CXX_INCLUDE_DIRS
    ${PROJECT_BINARY_DIR}/${THIRDPARTY_PREFIX}/src/hdf5_static/c++/src)
  set(HDF5_STATIC_INCLUDE_DIRS ${HDF5_STATIC_CXX_INCLUDE_DIRS})
  set(HDF5_STATIC_C_LIBRARIES ${PROJECT_BINARY_DIR}/${THIRDPARTY_PREFIX}/src/hdf5_static-build/bin/${CMAKE_STATIC_LIBRARY_PREFIX}hdf5${CMAKE_STATIC_LIBRARY_SUFFIX})
  list(APPEND HDF5_STATIC_C_LIBRARIES ${ZLIB_STATIC_LIBRARY})
  set(HDF5_STATIC_CXX_LIBRARIES ${PROJECT_BINARY_DIR}/${THIRDPARTY_PREFIX}/src/hdf5_static-build/bin/${CMAKE_STATIC_LIBRARY_PREFIX}hdf5_cpp${CMAKE_STATIC_LIBRARY_SUFFIX})
  list(APPEND HDF5_STATIC_CXX_LIBRARIES ${ZLIB_STATIC_LIBRARY})

  find_static_library(${BLITZ_LIBRARY} "BLITZ" "")
  find_static_library(${PNG_LIBRARY} "PNG" "")
  find_static_library(${TIFF_LIBRARY} "TIFF" "")
  find_static_library(${JPEG_LIBRARY} "JPEG" "")
  find_static_library(${FFTW3_LIBRARY} "FFTW3" "")
  find_static_library(${FFTW3F_LIBRARY} "FFTW3F" "")
  find_static_library(${GSL_LIBRARY} "GSL" "")
  find_static_library(${GSL_CBLAS_LIBRARY} "GSL_CBLAS" "")
  set(GSL_STATIC_LIBRARIES ${GSL_STATIC_LIBRARY} ${GSL_CBLAS_STATIC_LIBRARY})

  # Build our own mini OpenCV to reduce dependencies to a minimum
  set(OpenCV_URL "https://github.com/opencv/opencv/archive/2.4.13.2.zip")
  set(OpenCV_URL_MD5 "e48803864e77fc8ae7114be4de732d80")
  list(APPEND OpenCV_CMAKE_ARGS "-DBUILD_DOCS:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DBUILD_EXAMPLES:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DBUILD_JASPER:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DBUILD_JPEG:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DBUILD_OPENEXR:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DBUILD_PERF_TESTS:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DBUILD_PNG:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DBUILD_SHARED_LIBS:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DBUILD_TBB:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DBUILD_TESTS:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DBUILD_TIFF:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DBUILD_TINY_GPU_MODULE:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DBUILD_WITH_DEBUG_INFO:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DBUILD_ZLIB:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DBUILD_opencv_apps:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DBUILD_opencv_calib3d:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DBUILD_opencv_contrib:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DBUILD_opencv_core:BOOL=ON")
  list(APPEND OpenCV_CMAKE_ARGS "-DBUILD_opencv_features2d:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DBUILD_opencv_flann:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DBUILD_opencv_gpu:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DBUILD_opencv_highgui:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DBUILD_opencv_imgproc:BOOL=ON")
  list(APPEND OpenCV_CMAKE_ARGS "-DBUILD_opencv_java:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DBUILD_opencv_legacy:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DBUILD_opencv_ml:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DBUILD_opencv_nonfree:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DBUILD_opencv_objdetect:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DBUILD_opencv_ocl:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DBUILD_opencv_photo:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DBUILD_opencv_python:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DBUILD_opencv_stitching:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DBUILD_opencv_superres:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DBUILD_opencv_ts:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DBUILD_opencv_video:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DBUILD_opencv_videostab:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DBUILD_opencv_world:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DCMAKE_BUILD_TYPE:STRING=Release")
  list(APPEND OpenCV_CMAKE_ARGS "-DCMAKE_CONFIGURATION_TYPES:STRING=Release")
  list(APPEND OpenCV_CMAKE_ARGS "-DCMAKE_VERBOSE:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DWITH_1394:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DWITH_CUBLAS:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DWITH_CUDA:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DWITH_CUFFT:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DWITH_EIGEN:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DWITH_FFMPEG:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DWITH_GIGEAPI:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DWITH_GSTREAMER:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DWITH_GSTREAMER_0_10:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DWITH_GTK:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DWITH_IPP:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DWITH_JASPER:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DWITH_JPEG:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DWITH_LIBV4L:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DWITH_NVCUVID:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DWITH_OPENCL:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DWITH_OPENCLAMDBLAS:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DWITH_OPENCLAMDFFT:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DWITH_OPENEXR:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DWITH_OPENGL:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DWITH_OPENMP:BOOL=ON")
  list(APPEND OpenCV_CMAKE_ARGS "-DWITH_OPENNI:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DWITH_PNG:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DWITH_PVAPI:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DWITH_QT:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DWITH_TBB:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DWITH_TIFF:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DWITH_UNICAP:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DWITH_V4L:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DWITH_VTK:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DWITH_XIMEA:BOOL=OFF")
  list(APPEND OpenCV_CMAKE_ARGS "-DWITH_XINE:BOOL=OFF")
  ExternalProject_Add(
    OpenCV_static
    PREFIX ${THIRDPARTY_PREFIX}
    URL ${OpenCV_URL}
    URL_MD5 ${OpenCV_URL_MD5}
    CMAKE_ARGS ${OpenCV_CMAKE_ARGS}
    INSTALL_COMMAND "")
  list(APPEND OpenCV_STATIC_INCLUDE_DIRS
    ${PROJECT_BINARY_DIR}/${THIRDPARTY_PREFIX}/src/OpenCV_static/src)
  list(APPEND OpenCV_STATIC_INCLUDE_DIRS
    ${PROJECT_BINARY_DIR}/${THIRDPARTY_PREFIX}/src/OpenCV_static-build)
  list(APPEND OpenCV_STATIC_LIBRARIES ${PROJECT_BINARY_DIR}/${THIRDPARTY_PREFIX}/src/OpenCV_static-build/lib/${CMAKE_STATIC_LIBRARY_PREFIX}opencv_imgproc${CMAKE_STATIC_LIBRARY_SUFFIX})
  list(APPEND OpenCV_STATIC_LIBRARIES ${PROJECT_BINARY_DIR}/${THIRDPARTY_PREFIX}/src/OpenCV_static-build/lib/${CMAKE_STATIC_LIBRARY_PREFIX}opencv_core${CMAKE_STATIC_LIBRARY_SUFFIX})
  list(APPEND OpenCV_STATIC_LIBRARIES ${ZLIB_STATIC_LIBRARY})

endif()

# ===============================
# Write configuration header
# ===============================
configure_file(
  "${PROJECT_SOURCE_DIR}/src/config-cmake.hh.in"
  "${PROJECT_BINARY_DIR}/src/config.hh" )
add_definitions(-DHAVE_CONFIG_H)
include_directories(
  "${PROJECT_SOURCE_DIR}/src"
  "${PROJECT_BINARY_DIR}/src")
if (BUILD_SHARED_LIBS OR BUILD_STATIC_LIBS)
  install(FILES "${PROJECT_BINARY_DIR}/src/config.hh"
    DESTINATION include/${PACKAGE})
endif()

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

# =================================
# Create iRoCS-Toolbox-config.cmake
# =================================

if (BUILD_SHARED_LIBS OR BUILD_STATIC_LIBS)

  # Add all targets to the build-tree export set
  if (BUILD_SHARED_LIBS)
    export(
      TARGETS cmdline BaseFunctions ProgressReporter BlitzAnalyze
      Blitz2DGraphics BlitzFFTW BlitzHdf5 svmtl lmbs2kit ArrayToolbox
      segmentation IRoCS ProgressReporter_qt4 Labelling_qt4
      FILE "${PROJECT_BINARY_DIR}/iRoCS-ToolboxTargets.cmake"
      NAMESPACE iRoCS-Toolbox::)
    if (Qt5Widgets_FOUND)
      export(TARGETS ProgressReporter_qt5
	APPEND FILE "${PROJECT_BINARY_DIR}/iRoCS-ToolboxTargets.cmake"
	NAMESPACE iRoCS-Toolbox::)
    endif()
  endif()

  if (BUILD_STATIC_LIBS)
    export(
      TARGETS cmdline_static BaseFunctions_static ProgressReporter_static
      BlitzAnalyze_static Blitz2DGraphics_static BlitzFFTW_static
      BlitzHdf5_static svmtl_static lmbs2kit_static ArrayToolbox_static
      segmentation_static IRoCS_static ProgressReporter_qt4_static
      Labelling_qt4_static
      APPEND FILE "${PROJECT_BINARY_DIR}/iRoCS-ToolboxTargets.cmake"
      NAMESPACE iRoCS-Toolbox::)
    if (Qt5Widgets_FOUND)
      export(TARGETS ProgressReporter_qt5_static
	APPEND FILE "${PROJECT_BINARY_DIR}/iRoCS-ToolboxTargets.cmake"
	NAMESPACE iRoCS-Toolbox::)
    endif()
  endif()

  # Export package to use from build tree
  export(PACKAGE iRoCS-Toolbox)

  # Create iRoCS-ToolboxConfig.cmake and iRoCS-ToolboxConfigVersion files
  file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}"
    "${INSTALL_INCLUDE_DIR}")

  # ... for the build tree
  set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
  configure_file(iRoCS-ToolboxConfig.cmake.in
    "${PROJECT_BINARY_DIR}/iRoCS-ToolboxConfig.cmake" @ONLY)

  # ... for the install tree
  set(CONF_INCLUDE_DIRS "\${iRoCS-Toolbox_CMAKE_DIR}/${REL_INCLUDE_DIR}")
  configure_file(iRoCS-ToolboxConfig.cmake.in
    "${PROJECT_BINARY_DIR}/${CMAKE_FILES_DIRECTORY}/iRoCS-ToolboxConfig.cmake"
    @ONLY)

  # ... for both
  configure_file(iRoCS-ToolboxConfigVersion.cmake.in
    "${PROJECT_BINARY_DIR}/iRoCS-ToolboxConfigVersion.cmake" @ONLY)

  # Install config files
  install(FILES
    "${PROJECT_BINARY_DIR}/${CMAKE_FILES_DIRECTORY}/iRoCS-ToolboxConfig.cmake"
    "${PROJECT_BINARY_DIR}/iRoCS-ToolboxConfigVersion.cmake"
    DESTINATION "${INSTALL_CMAKE_DIR}")

  # Install the export set for use with the install tree
  install(EXPORT iRoCS-ToolboxTargets
    DESTINATION "${INSTALL_CMAKE_DIR}"
    NAMESPACE iRoCS-Toolbox::)

endif()
