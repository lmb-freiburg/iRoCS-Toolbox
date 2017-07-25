set(PluginLayerAssignment_VERSION_MAJOR 1)
set(PluginLayerAssignment_VERSION_MINOR 0)
set(PluginLayerAssignment_VERSION_PATCH 1)
set(PluginLayerAssignment_VERSION ${PluginLayerAssignment_VERSION_MAJOR}.${PluginLayerAssignment_VERSION_MINOR}.${PluginLayerAssignment_VERSION_PATCH})

set(CMAKE_AUTOMOC ON)

set(PluginLayerAssignment_HEADERS
  LayerAssignmentParametersDialog.hh LayerAssignmentThread.hh
  PluginLayerAssignment.hh)
set(PluginLayerAssignment_SOURCES
  LayerAssignmentParametersDialog.cc LayerAssignmentThread.cc
  PluginLayerAssignment.cc)

add_library(PluginLayerAssignment MODULE
  ${PluginLayerAssignment_SOURCES} ${PluginLayerAssignment_HEADERS})

target_link_libraries(PluginLayerAssignment PRIVATE Labelling_qt4 IRoCS)

install(TARGETS PluginLayerAssignment
  LIBRARY DESTINATION share/${PACKAGE}/plugins NAMELINK_SKIP
  RUNTIME DESTINATION share/${PACKAGE}/plugins)

set(CMAKE_AUTOMOC OFF)
