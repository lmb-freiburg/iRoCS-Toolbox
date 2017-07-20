set(PluginConvertMasksToMarkers_VERSION_MAJOR 1)
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

add_library(PluginConvertMasksToMarkers
  ${PluginConvertMasksToMarkers_SOURCES} ${PluginConvertMasksToMarkers_HEADERS})
target_link_libraries(PluginConvertMasksToMarkers Labelling_qt4)

# Install development headers
install(FILES ${PluginConvertMasksToMarkers_HEADERS}
  DESTINATION include/${PACKAGE}/plugins/libPluginConvertMasksToMarkers)

# Install libraries
if(WIN32)
  # On windows store DLLs alongside the binaries
  install(TARGETS PluginConvertMasksToMarkers
    LIBRARY DESTINATION share/${PACKAGE}/plugins NAMELINK_SKIP
    RUNTIME DESTINATION share/${PACKAGE}/plugins
    ARCHIVE DESTINATION lib)
else()
  install(TARGETS PluginConvertMasksToMarkers
    LIBRARY DESTINATION share/${PACKAGE}/plugins NAMELINK_SKIP
    ARCHIVE DESTINATION lib)
endif(WIN32)

# Explicitly add static library build if both static and shared libraries are
# requested
if(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)
  add_library(PluginConvertMasksToMarkers_static STATIC
    ${PluginConvertMasksToMarkers_SOURCES} ${PluginConvertMasksToMarkers_HEADERS})
  target_link_libraries(PluginConvertMasksToMarkers_static Labelling_qt4)
  install(TARGETS PluginConvertMasksToMarkers_static ARCHIVE DESTINATION lib)

  # On Linux rename static library to match shared library name
  if(UNIX)
    SET_TARGET_PROPERTIES(PluginConvertMasksToMarkers_static PROPERTIES
      OUTPUT_NAME PluginConvertMasksToMarkers CLEAN_DIRECT_OUTPUT 1)
  endif(UNIX)

endif(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)

set(CMAKE_AUTOMOC OFF)
