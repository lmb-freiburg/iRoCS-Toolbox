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

add_library(PluginTrainLayerAssignment MODULE
  ${PluginTrainLayerAssignment_SOURCES} ${PluginTrainLayerAssignment_HEADERS})

target_link_libraries(PluginTrainLayerAssignment PRIVATE Labelling_qt4 IRoCS)

install(TARGETS PluginTrainLayerAssignment
  LIBRARY DESTINATION share/${PACKAGE}/plugins NAMELINK_SKIP
  RUNTIME DESTINATION share/${PACKAGE}/plugins)

set(CMAKE_AUTOMOC OFF)
