set(ProgressReporter_qt5_VERSION_MAJOR 1)
set(ProgressReporter_qt5_VERSION_MINOR 0)
set(ProgressReporter_qt5_VERSION_PATCH 0)
set(ProgressReporter_qt5_VERSION ${ProgressReporter_qt5_VERSION_MAJOR}.${ProgressReporter_qt5_VERSION_MINOR}.${ProgressReporter_qt5_VERSION_PATCH})

set(ProgressReporter_qt5_HEADERS ProgressReporterQt5.hh)
set(ProgressReporter_qt5_SOURCES ProgressReporterQt5.cc)

set(CMAKE_AUTOMOC ON)

if (BUILD_SHARED_LIBS OR BUILD_STATIC_LIBS)
  # Install development headers
  install(FILES ${ProgressReporter_qt5_HEADERS}
    DESTINATION include/${PACKAGE}/libProgressReporter_qt5)
endif()

# Build shared library if requested
if (BUILD_SHARED_LIBS)
  add_library(ProgressReporter_qt5 SHARED
    ${ProgressReporter_qt5_SOURCES} ${ProgressReporter_qt5_HEADERS})
  set_target_properties(ProgressReporter_qt5 PROPERTIES
    VERSION ${ProgressReporter_qt5_VERSION}
    SOVERSION ${ProgressReporter_qt5_VERSION_MAJOR})
  target_link_libraries(ProgressReporter_qt5
    PUBLIC Qt5::Widgets ProgressReporter)
  install(TARGETS ProgressReporter_qt5
    EXPORT iRoCS-ToolboxTargets
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include/${PACKAGE}/libProgressReporter_qt5)
endif()

# Build static library if requested
if(BUILD_STATIC_LIBS)
  add_library(ProgressReporter_qt5_static STATIC
    ${ProgressReporter_qt5_SOURCES} ${ProgressReporter_qt5_HEADERS})
  target_link_libraries(ProgressReporter_qt5_static
    PUBLIC Qt5::Widgets ProgressReporter_static)
  install(TARGETS ProgressReporter_qt5_static
    EXPORT iRoCS-ToolboxTargets
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include/${PACKAGE}/libProgressReporter_qt5)

  # On Linux rename static library to match shared library name
  if(UNIX)
    SET_TARGET_PROPERTIES(ProgressReporter_qt5_static PROPERTIES
      OUTPUT_NAME ProgressReporter_qt5)
  endif()
endif()

set(CMAKE_AUTOMOC OFF)
