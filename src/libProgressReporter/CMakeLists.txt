set(ProgressReporter_VERSION_MAJOR 1) # bump to 2.0.0 at next release
set(ProgressReporter_VERSION_MINOR 0)
set(ProgressReporter_VERSION_PATCH 0)
set(ProgressReporter_VERSION ${ProgressReporter_VERSION_MAJOR}.${ProgressReporter_VERSION_MINOR}.${ProgressReporter_VERSION_PATCH})

set(ProgressReporter_HEADERS ProgressReporter.hh ProgressReporterStream.hh)
set(ProgressReporter_SOURCES ProgressReporter.cc ProgressReporterStream.cc)

add_library(ProgressReporter
  ${ProgressReporter_SOURCES} ${ProgressReporter_HEADERS})
set_target_properties(ProgressReporter PROPERTIES
  VERSION ${ProgressReporter_VERSION}
  SOVERSION ${ProgressReporter_VERSION_MAJOR})

# Install development headers
install(FILES ${ProgressReporter_HEADERS}
  DESTINATION include/${PACKAGE}/libProgressReporter)

# Install libraries
if(WIN32)
  # On windows store DLLs alongside the binaries
  install(TARGETS ProgressReporter
    LIBRARY DESTINATION bin
    ARCHIVE DESTINATION lib)
else()
  install(TARGETS ProgressReporter
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
endif()

# Explicitly add static library build if both static and shared libraries
# are requested
if(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)
  add_library(ProgressReporter_static STATIC
    ${ProgressReporter_SOURCES} ${ProgressReporter_HEADERS})
  install(TARGETS ProgressReporter_static ARCHIVE DESTINATION lib)

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
