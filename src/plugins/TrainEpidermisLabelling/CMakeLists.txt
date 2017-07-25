set(PluginTrainEpidermisLabelling_VERSION_MAJOR 2)
set(PluginTrainEpidermisLabelling_VERSION_MINOR 0)
set(PluginTrainEpidermisLabelling_VERSION_PATCH 0)
set(PluginTrainEpidermisLabelling_VERSION ${PluginTrainEpidermisLabelling_VERSION_MAJOR}.${PluginTrainEpidermisLabelling_VERSION_MINOR}.${PluginTrainEpidermisLabelling_VERSION_PATCH})

set(CMAKE_AUTOMOC ON)

set(PluginTrainEpidermisLabelling_HEADERS
  TrainEpidermisLabellingTrainfileParametersItem.hh
  TrainEpidermisLabellingParametersDialog.hh TrainEpidermisLabellingThread.hh
  PluginTrainEpidermisLabelling.hh)
set(PluginTrainEpidermisLabelling_SOURCES
  TrainEpidermisLabellingTrainfileParametersItem.cc
  TrainEpidermisLabellingParametersDialog.cc TrainEpidermisLabellingThread.cc
  PluginTrainEpidermisLabelling.cc)

add_library(PluginTrainEpidermisLabelling MODULE
  ${PluginTrainEpidermisLabelling_SOURCES}
  ${PluginTrainEpidermisLabelling_HEADERS})

target_link_libraries(PluginTrainEpidermisLabelling
  PRIVATE Labelling_qt4 IRoCS)

install(TARGETS PluginTrainEpidermisLabelling
  LIBRARY DESTINATION share/${PACKAGE}/plugins NAMELINK_SKIP
  RUNTIME DESTINATION share/${PACKAGE}/plugins)

set(CMAKE_AUTOMOC OFF)
