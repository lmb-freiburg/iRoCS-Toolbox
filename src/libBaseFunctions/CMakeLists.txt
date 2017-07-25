set(BaseFunctions_VERSION_MAJOR 2)
set(BaseFunctions_VERSION_MINOR 0)
set(BaseFunctions_VERSION_PATCH 0)
set(BaseFunctions_VERSION
  ${BaseFunctions_VERSION_MAJOR}.${BaseFunctions_VERSION_MINOR}.${BaseFunctions_VERSION_PATCH})

set(BaseFunctions_HEADERS BaseEnvironment.hh BaseFile.hh)
set(BaseFunctions_SOURCES BaseEnvironment.cc BaseFile.cc)

if (BUILD_SHARED_LIBS OR BUILD_STATIC_LIBS)
  # Install development headers
  install(FILES ${BaseFunctions_HEADERS}
    DESTINATION include/${PACKAGE}/libBaseFunctions)
endif()

# Build shared library if requested
if (BUILD_SHARED_LIBS)
  add_library(BaseFunctions SHARED
    ${BaseFunctions_SOURCES} ${BaseFunctions_HEADERS})
  set_target_properties(BaseFunctions PROPERTIES
    VERSION ${BaseFunctions_VERSION} SOVERSION ${BaseFunctions_VERSION_MAJOR})
  if (WIN32)
    target_compile_definitions(BaseFunctions INTERFACE BaseFunctions_USE_DLL)
  endif()
  install(TARGETS BaseFunctions
    EXPORT iRoCS-ToolboxTargets
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include/${PACKAGE}/libBaseFunctions)
endif()

# Build static library if requested
if(BUILD_STATIC_LIBS)
  add_library(BaseFunctions_static STATIC
    ${BaseFunctions_SOURCES} ${BaseFunctions_HEADERS})
  install(TARGETS BaseFunctions_static
    EXPORT iRoCS-ToolboxTargets
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include/${PACKAGE}/libBaseFunctions)

  # On Linux rename static library to match shared library name
  if(UNIX)
    SET_TARGET_PROPERTIES(BaseFunctions_static PROPERTIES
      OUTPUT_NAME BaseFunctions)
  endif()
endif()

# Explicitly add static library build for static binaries
if(BUILD_STATIC_TOOLS)
  add_library(BaseFunctions_static_tools STATIC
    ${BaseFunctions_SOURCES} ${BaseFunctions_HEADERS})
endif()
