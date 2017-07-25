set(PluginDetectNuclei_VERSION_MAJOR 2)
set(PluginDetectNuclei_VERSION_MINOR 0)
set(PluginDetectNuclei_VERSION_PATCH 0)
set(PluginDetectNuclei_VERSION ${PluginDetectNuclei_VERSION_MAJOR}.${PluginDetectNuclei_VERSION_MINOR}.${PluginDetectNuclei_VERSION_PATCH})

set(CMAKE_AUTOMOC ON)

set(PluginDetectNuclei_HEADERS
  DetectNucleiParametersDialog.hh DetectNucleiThread.hh PluginDetectNuclei.hh)
set(PluginDetectNuclei_SOURCES
  DetectNucleiParametersDialog.cc DetectNucleiThread.cc PluginDetectNuclei.cc)

add_library(PluginDetectNuclei MODULE
  ${PluginDetectNuclei_SOURCES} ${PluginDetectNuclei_HEADERS})

target_link_libraries(PluginDetectNuclei PRIVATE Labelling_qt4 IRoCS)

install(TARGETS PluginDetectNuclei
  LIBRARY DESTINATION share/${PACKAGE}/plugins NAMELINK_SKIP
  RUNTIME DESTINATION share/${PACKAGE}/plugins)

set(CMAKE_AUTOMOC OFF)
