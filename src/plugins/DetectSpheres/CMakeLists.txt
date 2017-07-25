set(PluginDetectSpheres_VERSION_MAJOR 1)
set(PluginDetectSpheres_VERSION_MINOR 0)
set(PluginDetectSpheres_VERSION_PATCH 0)
set(PluginDetectSpheres_VERSION ${PluginDetectSpheres_VERSION_MAJOR}.${PluginDetectSpheres_VERSION_MINOR}.${PluginDetectSpheres_VERSION_PATCH})

set(CMAKE_AUTOMOC ON)

set(PluginDetectSpheres_HEADERS
  DetectSpheresParametersDialog.hh DetectSpheresThread.hh PluginDetectSpheres.hh)
set(PluginDetectSpheres_SOURCES
  DetectSpheresParametersDialog.cc DetectSpheresThread.cc PluginDetectSpheres.cc)

add_library(PluginDetectSpheres MODULE
  ${PluginDetectSpheres_SOURCES} ${PluginDetectSpheres_HEADERS})

target_link_libraries(PluginDetectSpheres PRIVATE Labelling_qt4 IRoCS)

install(TARGETS PluginDetectSpheres
  LIBRARY DESTINATION share/${PACKAGE}/plugins NAMELINK_SKIP
  RUNTIME DESTINATION share/${PACKAGE}/plugins)

set(CMAKE_AUTOMOC OFF)
