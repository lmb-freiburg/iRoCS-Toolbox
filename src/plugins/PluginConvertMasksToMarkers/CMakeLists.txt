set(PluginConvertMasksToMarkers_VERSION_MAJOR 2)
set(PluginConvertMasksToMarkers_VERSION_MINOR 0)
set(PluginConvertMasksToMarkers_VERSION_PATCH 0)
set(PluginConvertMasksToMarkers_VERSION ${PluginConvertMasksToMarkers_VERSION_MAJOR}.${PluginConvertMasksToMarkers_VERSION_MINOR}.${PluginConvertMasksToMarkers_VERSION_PATCH})

set(CMAKE_AUTOMOC ON)

set(PluginConvertMasksToMarkers_HEADERS
  ConvertMasksToMarkersParameters.hh ConvertMasksToMarkersWorker.hh
  PluginConvertMasksToMarkers.hh)
set(PluginConvertMasksToMarkers_SOURCES
  ConvertMasksToMarkersParameters.cc ConvertMasksToMarkersWorker.cc
  PluginConvertMasksToMarkers.cc)

add_library(PluginConvertMasksToMarkers MODULE
  ${PluginConvertMasksToMarkers_SOURCES}
  ${PluginConvertMasksToMarkers_HEADERS})

target_link_libraries(PluginConvertMasksToMarkers PRIVATE Labelling_qt4)

install(TARGETS PluginConvertMasksToMarkers
  LIBRARY DESTINATION share/${PACKAGE}/plugins NAMELINK_SKIP
  RUNTIME DESTINATION share/${PACKAGE}/plugins)

set(CMAKE_AUTOMOC OFF)
