set(BlitzFFTW_VERSION_MAJOR 1)
set(BlitzFFTW_VERSION_MINOR 0)
set(BlitzFFTW_VERSION_PATCH 1)
set(BlitzFFTW_VERSION ${BlitzFFTW_VERSION_MAJOR}.${BlitzFFTW_VERSION_MINOR}.${BlitzFFTW_VERSION_PATCH})

set(BlitzFFTW_HEADERS
  BlitzFFTW.hh BlitzFFTW.icc BlitzFFTW-Impl.hh BlitzFFTWError.hh)
set(BlitzFFTW_SOURCES BlitzFFTW.cc BlitzFFTW-Impl.cc)

if (BUILD_SHARED_LIBS OR BUILD_STATIC_LIBS)
  # Install development headers
  install(FILES ${BlitzFFTW_HEADERS}
    DESTINATION include/${PACKAGE}/libBlitzFFTW)
endif()

# Build shared library if requested
if (BUILD_SHARED_LIBS)
  add_library(BlitzFFTW SHARED ${BlitzFFTW_SOURCES} ${BlitzFFTW_HEADERS})
  set_target_properties(BlitzFFTW PROPERTIES
    VERSION ${BlitzFFTW_VERSION} SOVERSION ${BlitzFFTW_VERSION_MAJOR})
  target_include_directories(BlitzFFTW
    PUBLIC ${BLITZ_INCLUDE_DIRS} ${FFTW3_INCLUDE_DIRS} ${FFTW3F_INCLUDE_DIRS})
  target_link_libraries(BlitzFFTW
    PUBLIC ${BLITZ_LIBRARIES} ${FFTW3_LIBRARIES} ${FFTW3F_LIBRARIES}
    BaseFunctions)
  install(TARGETS BlitzFFTW
    EXPORT iRoCS-ToolboxTargets
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include/${PACKAGE}/libBlitzFFTW)
endif()

if(BUILD_STATIC_LIBS)
  add_library(BlitzFFTW_static STATIC ${BlitzFFTW_SOURCES} ${BlitzFFTW_HEADERS})
  target_include_directories(BlitzFFTW_static
    PUBLIC ${BLITZ_INCLUDE_DIRS} ${FFTW3_INCLUDE_DIRS} ${FFTW3F_INCLUDE_DIRS})
  target_link_libraries(BlitzFFTW_static
    PUBLIC ${BLITZ_LIBRARIES} ${FFTW3_LIBRARIES} ${FFTW3F_LIBRARIES}
    BaseFunctions_static)
  install(TARGETS BlitzFFTW_static
    EXPORT iRoCS-ToolboxTargets
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include/${PACKAGE}/libBlitzFFTW)

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
    PUBLIC ${BLITZ_STATIC_LIBRARIES} ${FFTW3_STATIC_LIBRARIES}
    ${FFTW3F_STATIC_LIBRARIES} BaseFunctions_static_tools)
endif()
