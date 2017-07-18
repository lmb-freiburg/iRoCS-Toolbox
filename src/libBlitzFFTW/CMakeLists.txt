set(BlitzFFTW_VERSION_MAJOR 1)
set(BlitzFFTW_VERSION_MINOR 0)
set(BlitzFFTW_VERSION_PATCH 0)
set(BlitzFFTW_VERSION ${BlitzFFTW_VERSION_MAJOR}.${BlitzFFTW_VERSION_MINOR}.${BlitzFFTW_VERSION_PATCH})

set(BlitzFFTW_HEADERS
  BlitzFFTW.hh BlitzFFTW.icc BlitzFFTW-Impl.hh BlitzFFTWError.hh)
set(BlitzFFTW_SOURCES BlitzFFTW.cc BlitzFFTW-Impl.cc)

add_library(BlitzFFTW ${BlitzFFTW_SOURCES} ${BlitzFFTW_HEADERS})
set_target_properties(BlitzFFTW PROPERTIES
  VERSION ${BlitzFFTW_VERSION} SOVERSION ${BlitzFFTW_VERSION_MAJOR})
target_include_directories(BlitzFFTW
  PUBLIC ${BLITZ_INCLUDE_DIRS} ${FFTW3_INCLUDE_DIRS} ${FFTW3F_INCLUDE_DIRS})
target_link_libraries(BlitzFFTW
  ${BLITZ_LIBRARIES} ${FFTW3_LIBRARIES} ${FFTW3F_LIBRARIES} BaseFunctions)

# Install development headers
install(FILES ${BlitzFFTW_HEADERS} DESTINATION include/${PACKAGE}/libBlitzFFTW)

# Install libraries
if(WIN32)
  # On windows store DLLs alongside the binaries
  install(TARGETS BlitzFFTW
    LIBRARY DESTINATION bin
    ARCHIVE DESTINATION lib)
else()
  install(TARGETS BlitzFFTW
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
endif(WIN32)

# Explicitly add static library build if both static and shared libraries
# are requested
if(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)
  add_library(BlitzFFTW_static STATIC ${BlitzFFTW_SOURCES} ${BlitzFFTW_HEADERS})
  target_include_directories(BlitzFFTW_static
    PUBLIC ${BLITZ_INCLUDE_DIRS} ${FFTW3_INCLUDE_DIRS} ${FFTW3F_INCLUDE_DIRS})
  target_link_libraries(BlitzFFTW_static
    ${BLITZ_LIBRARIES} ${FFTW3_LIBRARIES} ${FFTW3F_LIBRARIES} BaseFunctions)
  install(TARGETS BlitzFFTW_static ARCHIVE DESTINATION lib)

  # On Linux rename static library to match shared library name
  if(UNIX)
    SET_TARGET_PROPERTIES(BlitzFFTW_static PROPERTIES OUTPUT_NAME BlitzFFTW)
  endif()
endif()

# Explicitly add static library build for static binaries
if(BUILD_STATIC_TOOLS)
  add_library(BlitzFFTW_static_tools
    STATIC ${BlitzFFTW_SOURCES} ${BlitzFFTW_HEADERS})
  target_include_directories(BlitzFFTW_static_tools
    PUBLIC ${BLITZ_INCLUDE_DIRS} ${FFTW3_INCLUDE_DIRS} ${FFTW3F_INCLUDE_DIRS})
  target_link_libraries(BlitzFFTW_static_tools
    ${BLITZ_STATIC_LIBRARIES} ${FFTW3_STATIC_LIBRARIES}
    ${FFTW3F_STATIC_LIBRARIES} BaseFunctions_static_tools)
endif()
