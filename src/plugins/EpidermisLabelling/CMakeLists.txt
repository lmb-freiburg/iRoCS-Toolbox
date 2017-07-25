set(PluginEpidermisLabelling_VERSION_MAJOR 1)
set(PluginEpidermisLabelling_VERSION_MINOR 0)
set(PluginEpidermisLabelling_VERSION_PATCH 1)
set(PluginEpidermisLabelling_VERSION ${PluginEpidermisLabelling_VERSION_MAJOR}.${PluginEpidermisLabelling_VERSION_MINOR}.${PluginEpidermisLabelling_VERSION_PATCH})

set(CMAKE_AUTOMOC ON)

set(PluginEpidermisLabelling_HEADERS
  EpidermisLabellingParametersDialog.hh EpidermisLabellingThread.hh
  PluginEpidermisLabelling.hh)
set(PluginEpidermisLabelling_SOURCES
  EpidermisLabellingParametersDialog.cc EpidermisLabellingThread.cc
  PluginEpidermisLabelling.cc)

add_library(PluginEpidermisLabelling MODULE
  ${PluginEpidermisLabelling_SOURCES} ${PluginEpidermisLabelling_HEADERS})

target_link_libraries(PluginEpidermisLabelling PRIVATE Labelling_qt4 IRoCS)

install(TARGETS PluginEpidermisLabelling
  LIBRARY DESTINATION share/${PACKAGE}/plugins NAMELINK_SKIP
  RUNTIME DESTINATION share/${PACKAGE}/plugins)

set(CMAKE_AUTOMOC OFF)
