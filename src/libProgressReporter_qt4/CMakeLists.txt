set(ProgressReporter_qt4_VERSION_MAJOR 1)
set(ProgressReporter_qt4_VERSION_MINOR 0)
set(ProgressReporter_qt4_VERSION_PATCH 0)
set(ProgressReporter_qt4_VERSION ${ProgressReporter_qt4_VERSION_MAJOR}.${ProgressReporter_qt4_VERSION_MINOR}.${ProgressReporter_qt4_VERSION_PATCH})

set(ProgressReporter_qt4_HEADERS ProgressReporterQt4.hh)
set(ProgressReporter_qt4_SOURCES ProgressReporterQt4.cc)

set(CMAKE_AUTOMOC ON)

if (BUILD_SHARED_LIBS OR BUILD_STATIC_LIBS)
# Install development headers
install(FILES ${ProgressReporter_qt4_HEADERS}
  DESTINATION include/${PACKAGE}/libProgressReporter_qt4)
endif()

# Build shared library if requested
if (BUILD_SHARED_LIBS)
  add_library(ProgressReporter_qt4 SHARED
    ${ProgressReporter_qt4_SOURCES} ${ProgressReporter_qt4_HEADERS})
  set_target_properties(ProgressReporter_qt4 PROPERTIES
    VERSION ${ProgressReporter_qt4_VERSION}
    SOVERSION ${ProgressReporter_qt4_VERSION_MAJOR})
  target_link_libraries(ProgressReporter_qt4 PUBLIC Qt4::QtGui ProgressReporter)
  install(TARGETS ProgressReporter_qt4
    EXPORT iRoCS-ToolboxTargets
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include/${PACKAGE}/libProgressReporter_qt4)
endif()

# Build static library if requested
if(BUILD_STATIC_LIBS)
  add_library(ProgressReporter_qt4_static STATIC
    ${ProgressReporter_qt4_SOURCES} ${ProgressReporter_qt4_HEADERS})
  target_link_libraries(ProgressReporter_qt4_static
    PUBLIC Qt4::QtGui ProgressReporter_static)
  install(TARGETS ProgressReporter_qt4_static
    EXPORT iRoCS-ToolboxTargets
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include/${PACKAGE}/libProgressReporter_qt4)

  # On Linux rename static library to match shared library name
  if(UNIX)
    SET_TARGET_PROPERTIES(ProgressReporter_qt4_static PROPERTIES
      OUTPUT_NAME ProgressReporter_qt4)
  endif(UNIX)
endif()

set(CMAKE_AUTOMOC OFF)
