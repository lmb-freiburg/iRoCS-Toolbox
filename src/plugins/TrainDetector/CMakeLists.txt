set(PluginTrainDetector_VERSION_MAJOR 2)
set(PluginTrainDetector_VERSION_MINOR 0)
set(PluginTrainDetector_VERSION_PATCH 0)
set(PluginTrainDetector_VERSION ${PluginTrainDetector_VERSION_MAJOR}.${PluginTrainDetector_VERSION_MINOR}.${PluginTrainDetector_VERSION_PATCH})

set(CMAKE_AUTOMOC ON)

set(PluginTrainDetector_HEADERS
  TrainDetectorTrainfileParametersItem.hh TrainDetectorParametersDialog.hh
  TrainDetectorThread.hh PluginTrainDetector.hh)
set(PluginTrainDetector_SOURCES
  TrainDetectorTrainfileParametersItem.cc TrainDetectorParametersDialog.cc
  TrainDetectorThread.cc PluginTrainDetector.cc)

add_library(PluginTrainDetector
  ${PluginTrainDetector_SOURCES} ${PluginTrainDetector_HEADERS})
target_link_libraries(PluginTrainDetector Labelling_qt4 IRoCS)

# Install development headers
install(FILES ${PluginTrainDetector_HEADERS}
  DESTINATION include/${PACKAGE}/plugins/libPluginTrainDetector)

# Install libraries
if(WIN32)
  # On windows store DLLs alongside the binaries
  install(TARGETS PluginTrainDetector
    LIBRARY DESTINATION share/${PACKAGE}/plugins NAMELINK_SKIP
    RUNTIME DESTINATION share/${PACKAGE}/plugins
    ARCHIVE DESTINATION lib)
else()
  install(TARGETS PluginTrainDetector
    LIBRARY DESTINATION share/${PACKAGE}/plugins NAMELINK_SKIP
    ARCHIVE DESTINATION lib)
endif(WIN32)

# Explicitly add static library build if both static and shared libraries are
# requested
if(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)
  add_library(PluginTrainDetector_static STATIC
    ${PluginTrainDetector_SOURCES} ${PluginTrainDetector_HEADERS})
  target_link_libraries(PluginTrainDetector_static Labelling_qt4 IRoCS)
  install(TARGETS PluginTrainDetector_static ARCHIVE DESTINATION lib)

  # On Linux rename static library to match shared library name
  if(UNIX)
    SET_TARGET_PROPERTIES(PluginTrainDetector_static PROPERTIES
      OUTPUT_NAME PluginTrainDetector CLEAN_DIRECT_OUTPUT 1)
  endif(UNIX)

endif(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)

set(CMAKE_AUTOMOC OFF)
