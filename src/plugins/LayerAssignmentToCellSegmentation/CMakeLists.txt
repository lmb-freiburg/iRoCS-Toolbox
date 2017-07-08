set(PluginAssignLayersToCellSegmentation_VERSION_MAJOR 1)
set(PluginAssignLayersToCellSegmentation_VERSION_MINOR 0)
set(PluginAssignLayersToCellSegmentation_VERSION_PATCH 0)
set(PluginAssignLayersToCellSegmentation_VERSION ${PluginAssignLayersToCellSegmentation_VERSION_MAJOR}.${PluginAssignLayersToCellSegmentation_VERSION_MINOR}.${PluginAssignLayersToCellSegmentation_VERSION_PATCH})

set(CMAKE_AUTOMOC ON)

set(PluginAssignLayersToCellSegmentation_HEADERS
  AssignLayersToCellSegmentationParametersDialog.hh
  AssignLayersToCellSegmentationThread.hh
  PluginAssignLayersToCellSegmentation.hh)
set(PluginAssignLayersToCellSegmentation_SOURCES
  AssignLayersToCellSegmentationParametersDialog.cc
  AssignLayersToCellSegmentationThread.cc
  PluginAssignLayersToCellSegmentation.cc)

add_library(PluginAssignLayersToCellSegmentation
  ${PluginAssignLayersToCellSegmentation_SOURCES}
  ${PluginAssignLayersToCellSegmentation_HEADERS})
target_link_libraries(PluginAssignLayersToCellSegmentation Labelling_qt4 IRoCS)

# Install development headers
install(FILES ${PluginAssignLayersToCellSegmentation_HEADERS}
  DESTINATION
  include/${PACKAGE}/plugins/libPluginAssignLayersToCellSegmentation)

# Install libraries
if(WIN32)
  # On windows store DLLs alongside the binaries
  install(TARGETS PluginAssignLayersToCellSegmentation
    LIBRARY DESTINATION share/${PACKAGE}/plugins NAMELINK_SKIP
    ARCHIVE DESTINATION lib)
else()
  install(TARGETS PluginAssignLayersToCellSegmentation
    LIBRARY DESTINATION share/${PACKAGE}/plugins NAMELINK_SKIP
    ARCHIVE DESTINATION lib)
endif(WIN32)

# Explicitly add static library build if both static and shared libraries are
# requested
if(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)
  add_library(PluginAssignLayersToCellSegmentation_static STATIC
    ${PluginAssignLayersToCellSegmentation_SOURCES}
    ${PluginAssignLayersToCellSegmentation_HEADERS})
  target_link_libraries(PluginAssignLayersToCellSegmentation_static
    Labelling_qt4 IRoCS)
  install(TARGETS PluginAssignLayersToCellSegmentation_static
    ARCHIVE DESTINATION lib)

  # On Linux rename static library to match shared library name
  if(UNIX)
    SET_TARGET_PROPERTIES(PluginAssignLayersToCellSegmentation_static
      PROPERTIES
      OUTPUT_NAME PluginAssignLayersToCellSegmentation CLEAN_DIRECT_OUTPUT 1)
  endif(UNIX)

endif(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)

set(CMAKE_AUTOMOC OFF)
