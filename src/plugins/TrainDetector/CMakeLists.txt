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

add_library(PluginTrainDetector MODULE
  ${PluginTrainDetector_SOURCES} ${PluginTrainDetector_HEADERS})

target_link_libraries(PluginTrainDetector PRIVATE Labelling_qt4 IRoCS)

install(TARGETS PluginTrainDetector
  LIBRARY DESTINATION share/${PACKAGE}/plugins NAMELINK_SKIP
  RUNTIME DESTINATION share/${PACKAGE}/plugins)

set(CMAKE_AUTOMOC OFF)
