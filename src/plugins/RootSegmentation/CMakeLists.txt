set(PluginRootSegmentation_VERSION_MAJOR 2)
set(PluginRootSegmentation_VERSION_MINOR 0)
set(PluginRootSegmentation_VERSION_PATCH 0)
set(PluginRootSegmentation_VERSION ${PluginRootSegmentation_VERSION_MAJOR}.${PluginRootSegmentation_VERSION_MINOR}.${PluginRootSegmentation_VERSION_PATCH})

set(CMAKE_AUTOMOC ON)

set(PluginRootSegmentation_HEADERS
  RootSegmentationParametersDialog.hh RootSegmentationThread.hh
  PluginRootSegmentation.hh)
set(PluginRootSegmentation_SOURCES
  RootSegmentationParametersDialog.cc RootSegmentationThread.cc
  PluginRootSegmentation.cc)

add_library(PluginRootSegmentation MODULE
  ${PluginRootSegmentation_SOURCES} ${PluginRootSegmentation_HEADERS})

target_link_libraries(PluginRootSegmentation PRIVATE Labelling_qt4 IRoCS)

install(TARGETS PluginRootSegmentation
  LIBRARY DESTINATION share/${PACKAGE}/plugins NAMELINK_SKIP
  RUNTIME DESTINATION share/${PACKAGE}/plugins)

set(CMAKE_AUTOMOC OFF)
