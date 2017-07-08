set(ProgressReporter_qt5_VERSION_MAJOR 1)
set(ProgressReporter_qt5_VERSION_MINOR 0)
set(ProgressReporter_qt5_VERSION_PATCH 0)
set(ProgressReporter_qt5_VERSION ${ProgressReporter_qt5_VERSION_MAJOR}.${ProgressReporter_qt5_VERSION_MINOR}.${ProgressReporter_qt5_VERSION_PATCH})

set(ProgressReporter_qt5_HEADERS ProgressReporterQt5.hh)
set(ProgressReporter_qt5_SOURCES ProgressReporterQt5.cc)

set(CMAKE_AUTOMOC ON)

add_library(ProgressReporter_qt5
  ${ProgressReporter_qt5_SOURCES} ${ProgressReporter_qt5_HEADERS})
set_target_properties(ProgressReporter_qt5 PROPERTIES
  VERSION ${ProgressReporter_qt5_VERSION}
  SOVERSION ${ProgressReporter_qt5_VERSION_MAJOR})
target_link_libraries(ProgressReporter_qt5 Qt5::Widgets ProgressReporter)

# Install development headers
install(FILES ${ProgressReporter_qt5_HEADERS}
  DESTINATION include/${PACKAGE}/libProgressReporter_qt5)

# Install libraries
if(WIN32)
  # On windows store DLLs alongside the binaries
  install(TARGETS ProgressReporter_qt5
    LIBRARY DESTINATION bin
    ARCHIVE DESTINATION lib)
else()
  install(TARGETS ProgressReporter_qt5
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
endif(WIN32)

# Explicitly add static library build if both static and shared libraries
# are requested
if(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)
  add_library(ProgressReporter_qt5_static STATIC
    ${ProgressReporter_qt5_SOURCES} ${ProgressReporter_qt5_HEADERS})
  target_link_libraries(ProgressReporter_qt5_static
    Qt5::Widgets ProgressReporter)
  install(TARGETS ProgressReporter_qt5_static ARCHIVE DESTINATION lib)

  # On Linux rename static library to match shared library name
  if(UNIX)
    SET_TARGET_PROPERTIES(ProgressReporter_qt5_static PROPERTIES
      OUTPUT_NAME ProgressReporter_qt5 CLEAN_DIRECT_OUTPUT 1)
  endif(UNIX)

endif(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)

set(CMAKE_AUTOMOC OFF)
