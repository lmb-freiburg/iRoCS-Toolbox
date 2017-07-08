set(BaseFunctions_VERSION_MAJOR 1)
set(BaseFunctions_VERSION_MINOR 0)
set(BaseFunctions_VERSION_PATCH 0)
set(BaseFunctions_VERSION
  ${BaseFunctions_VERSION_MAJOR}.${BaseFunctions_VERSION_MINOR}.${BaseFunctions_VERSION_PATCH})

set(BaseFunctions_HEADERS BaseEnvironment.hh BaseFile.hh)
set(BaseFunctions_SOURCES BaseEnvironment.cc BaseFile.cc)

add_library(BaseFunctions ${BaseFunctions_SOURCES} ${BaseFunctions_HEADERS})
set_target_properties(BaseFunctions PROPERTIES
  VERSION ${BaseFunctions_VERSION} SOVERSION ${BaseFunctions_VERSION_MAJOR})

# Install development headers
install(FILES ${BaseFunctions_HEADERS}
  DESTINATION include/${PACKAGE}/libBaseFunctions)

# Install libraries
if(WIN32)
  # On windows store DLLs alongside the binaries
  install(TARGETS BaseFunctions
    LIBRARY DESTINATION bin
    ARCHIVE DESTINATION lib)
else()
  install(TARGETS BaseFunctions
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
endif(WIN32)

# Explicitly add static library build if both static and shared libraries
# are requested
if(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)
  add_library(BaseFunctions_static STATIC
    ${BaseFunctions_SOURCES} ${BaseFunctions_HEADERS})
  install(TARGETS BaseFunctions_static ARCHIVE DESTINATION lib)

  # On Linux rename static library to match shared library name
  if(UNIX)
    SET_TARGET_PROPERTIES(BaseFunctions_static PROPERTIES
      OUTPUT_NAME BaseFunctions CLEAN_DIRECT_OUTPUT 1)
  endif(UNIX)

endif(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)
