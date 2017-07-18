set(ProgressReporter_qt4_VERSION_MAJOR 1)
set(ProgressReporter_qt4_VERSION_MINOR 0)
set(ProgressReporter_qt4_VERSION_PATCH 0)
set(ProgressReporter_qt4_VERSION ${ProgressReporter_qt4_VERSION_MAJOR}.${ProgressReporter_qt4_VERSION_MINOR}.${ProgressReporter_qt4_VERSION_PATCH})

set(ProgressReporter_qt4_HEADERS ProgressReporterQt4.hh)
set(ProgressReporter_qt4_SOURCES ProgressReporterQt4.cc)

set(CMAKE_AUTOMOC ON)

add_library(ProgressReporter_qt4
  ${ProgressReporter_qt4_SOURCES} ${ProgressReporter_qt4_HEADERS})
set_target_properties(ProgressReporter_qt4 PROPERTIES
  VERSION ${ProgressReporter_qt4_VERSION}
  SOVERSION ${ProgressReporter_qt4_VERSION_MAJOR})
target_link_libraries(ProgressReporter_qt4 Qt4::QtGui ProgressReporter)

# Install development headers
install(FILES ${ProgressReporter_qt4_HEADERS}
  DESTINATION include/${PACKAGE}/libProgressReporter_qt4)

# Install libraries
if(WIN32)
  # On windows store DLLs alongside the binaries
  install(TARGETS ProgressReporter_qt4
    LIBRARY DESTINATION bin
    ARCHIVE DESTINATION lib)
else()
  install(TARGETS ProgressReporter_qt4
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
endif(WIN32)

# Explicitly add static library build if both static and shared libraries
# are requested
if(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)
  add_library(ProgressReporter_qt4_static STATIC
    ${ProgressReporter_qt4_SOURCES} ${ProgressReporter_qt4_HEADERS})
  target_link_libraries(ProgressReporter_qt4_static Qt4::QtGui
    ProgressReporter_static)
  install(TARGETS ProgressReporter_qt4_static ARCHIVE DESTINATION lib)

  # On Linux rename static library to match shared library name
  if(UNIX)
    SET_TARGET_PROPERTIES(ProgressReporter_qt4_static PROPERTIES
      OUTPUT_NAME ProgressReporter_qt4 CLEAN_DIRECT_OUTPUT 1)
  endif(UNIX)

endif(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)

set(CMAKE_AUTOMOC OFF)
