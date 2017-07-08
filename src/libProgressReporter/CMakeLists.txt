add_library(ProgressReporter
  ProgressReporter.cc ProgressReporterStream.cc ProgressReporter.hh
  ProgressReporterStream.hh )
set(ProgressReporter_VERSION_MAJOR 1) # bump to 2.0.0 at next release
set(ProgressReporter_VERSION_MINOR 0)
set(ProgressReporter_VERSION_PATCH 0)
set(ProgressReporter_VERSION ${ProgressReporter_VERSION_MAJOR}.${ProgressReporter_VERSION_MINOR}.${ProgressReporter_VERSION_PATCH})
set_target_properties(ProgressReporter PROPERTIES VERSION ${ProgressReporter_VERSION} SOVERSION ${ProgressReporter_VERSION_MAJOR})

# Install development headers
install(DIRECTORY . DESTINATION include/libProgressReporter
  FILES_MATCHING PATTERN "*.hh" PATTERN "*.icc")

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
endif(WIN32)

# Explicitly add static library build if both static and shared libraries are requested
if(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)
  add_library(ProgressReporter_static STATIC
    ProgressReporter.cc ProgressReporterStream.cc ProgressReporter.hh
    ProgressReporterStream.hh )
  install(TARGETS ProgressReporter_static ARCHIVE DESTINATION lib)

  # On Linux rename static library to match shared library name
  if(UNIX)
    SET_TARGET_PROPERTIES(ProgressReporter_static PROPERTIES OUTPUT_NAME ProgressReporter CLEAN_DIRECT_OUTPUT 1)
  endif(UNIX)

endif(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)
