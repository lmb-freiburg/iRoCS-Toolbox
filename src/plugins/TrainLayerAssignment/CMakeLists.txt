set(PluginTrainLayerAssignment_VERSION_MAJOR 2)
set(PluginTrainLayerAssignment_VERSION_MINOR 0)
set(PluginTrainLayerAssignment_VERSION_PATCH 0)
set(PluginTrainLayerAssignment_VERSION ${PluginTrainLayerAssignment_VERSION_MAJOR}.${PluginTrainLayerAssignment_VERSION_MINOR}.${PluginTrainLayerAssignment_VERSION_PATCH})

set(CMAKE_AUTOMOC ON)

set(PluginTrainLayerAssignment_HEADERS
  TrainLayerAssignmentTrainfileParametersItem.hh
  TrainLayerAssignmentParametersDialog.hh TrainLayerAssignmentThread.hh
  PluginTrainLayerAssignment.hh)
set(PluginTrainLayerAssignment_SOURCES
  TrainLayerAssignmentTrainfileParametersItem.cc
  TrainLayerAssignmentParametersDialog.cc TrainLayerAssignmentThread.cc
  PluginTrainLayerAssignment.cc)

add_library(PluginTrainLayerAssignment
  ${PluginTrainLayerAssignment_SOURCES} ${PluginTrainLayerAssignment_HEADERS})
target_link_libraries(PluginTrainLayerAssignment Labelling_qt4 IRoCS)

# Install development headers
install(FILES ${PluginTrainLayerAssignment_HEADERS}
  DESTINATION include/${PACKAGE}/plugins/libPluginTrainLayerAssignment)

# Install libraries
if(WIN32)
  # On windows store DLLs alongside the binaries
  install(TARGETS PluginTrainLayerAssignment
    LIBRARY DESTINATION share/${PACKAGE}/plugins NAMELINK_SKIP
    RUNTIME DESTINATION share/${PACKAGE}/plugins
    ARCHIVE DESTINATION lib)
else()
  install(TARGETS PluginTrainLayerAssignment
    LIBRARY DESTINATION share/${PACKAGE}/plugins NAMELINK_SKIP
    ARCHIVE DESTINATION lib)
endif(WIN32)

# Explicitly add static library build if both static and shared libraries are
# requested
if(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)
  add_library(PluginTrainLayerAssignment_static STATIC
    ${PluginTrainLayerAssignment_SOURCES} ${PluginTrainLayerAssignment_HEADERS})
  target_link_libraries(PluginTrainLayerAssignment_static Labelling_qt4 IRoCS)
  install(TARGETS PluginTrainLayerAssignment_static ARCHIVE DESTINATION lib)

  # On Linux rename static library to match shared library name
  if(UNIX)
    SET_TARGET_PROPERTIES(PluginTrainLayerAssignment_static PROPERTIES
      OUTPUT_NAME PluginTrainLayerAssignment CLEAN_DIRECT_OUTPUT 1)
  endif(UNIX)

endif(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)

set(CMAKE_AUTOMOC OFF)
