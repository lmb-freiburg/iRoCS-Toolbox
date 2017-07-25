set(ProgressReporter_VERSION_MAJOR 2)
set(ProgressReporter_VERSION_MINOR 0)
set(ProgressReporter_VERSION_PATCH 0)
set(ProgressReporter_VERSION ${ProgressReporter_VERSION_MAJOR}.${ProgressReporter_VERSION_MINOR}.${ProgressReporter_VERSION_PATCH})

set(ProgressReporter_HEADERS ProgressReporter.hh ProgressReporterStream.hh)
set(ProgressReporter_SOURCES ProgressReporter.cc ProgressReporterStream.cc)

if (BUILD_SHARED_LIBS OR BUILD_STATIC_LIBS)
  # Install development headers
  install(FILES ${ProgressReporter_HEADERS}
    DESTINATION include/${PACKAGE}/libProgressReporter)
endif()

# Build shared library if requested
if (BUILD_SHARED_LIBS)
  add_library(ProgressReporter SHARED
    ${ProgressReporter_SOURCES} ${ProgressReporter_HEADERS})
  set_target_properties(ProgressReporter PROPERTIES
    VERSION ${ProgressReporter_VERSION}
    SOVERSION ${ProgressReporter_VERSION_MAJOR})
  install(TARGETS ProgressReporter
    EXPORT iRoCS-ToolboxTargets
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include/${PACKAGE}/libProgressReporter)
endif()

# Build static library if requested
if(BUILD_STATIC_LIBS)
  add_library(ProgressReporter_static STATIC
    ${ProgressReporter_SOURCES} ${ProgressReporter_HEADERS})
  install(TARGETS ProgressReporter_static
    EXPORT iRoCS-ToolboxTargets
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include/${PACKAGE}/libProgressReporter)

  # On Linux rename static library to match shared library name
  if(UNIX)
    SET_TARGET_PROPERTIES(ProgressReporter_static PROPERTIES
      OUTPUT_NAME ProgressReporter)
  endif()
endif()

# Explicitly add static library build for static binaries
if(BUILD_STATIC_TOOLS)
  add_library(ProgressReporter_static_tools STATIC
    ${ProgressReporter_SOURCES} ${ProgressReporter_HEADERS})
endif()
