set(PluginLayerAssignment_VERSION_MAJOR 1)
set(PluginLayerAssignment_VERSION_MINOR 0)
set(PluginLayerAssignment_VERSION_PATCH 0)
set(PluginLayerAssignment_VERSION ${PluginLayerAssignment_VERSION_MAJOR}.${PluginLayerAssignment_VERSION_MINOR}.${PluginLayerAssignment_VERSION_PATCH})

set(CMAKE_AUTOMOC ON)

set(PluginLayerAssignment_HEADERS
  LayerAssignmentParametersDialog.hh LayerAssignmentThread.hh
  PluginLayerAssignment.hh)
set(PluginLayerAssignment_SOURCES
  LayerAssignmentParametersDialog.cc LayerAssignmentThread.cc
  PluginLayerAssignment.cc)

add_library(PluginLayerAssignment
  ${PluginLayerAssignment_SOURCES} ${PluginLayerAssignment_HEADERS})
target_link_libraries(PluginLayerAssignment Labelling_qt4 IRoCS)

# Install development headers
install(FILES ${PluginLayerAssignment_HEADERS}
  DESTINATION include/${PACKAGE}/plugins/libPluginLayerAssignment)

# Install libraries
if(WIN32)
  # On windows store DLLs alongside the binaries
  install(TARGETS PluginLayerAssignment
    LIBRARY DESTINATION share/${PACKAGE}/plugins NAMELINK_SKIP
    RUNTIME DESTINATION share/${PACKAGE}/plugins
    ARCHIVE DESTINATION lib)
else()
  install(TARGETS PluginLayerAssignment
    LIBRARY DESTINATION share/${PACKAGE}/plugins NAMELINK_SKIP
    ARCHIVE DESTINATION lib)
endif(WIN32)

# Explicitly add static library build if both static and shared libraries are
# requested
if(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)
  add_library(PluginLayerAssignment_static STATIC
    ${PluginLayerAssignment_SOURCES} ${PluginLayerAssignment_HEADERS})
  target_link_libraries(PluginLayerAssignment_static Labelling_qt4 IRoCS)
  install(TARGETS PluginLayerAssignment_static ARCHIVE DESTINATION lib)

  # On Linux rename static library to match shared library name
  if(UNIX)
    SET_TARGET_PROPERTIES(PluginLayerAssignment_static PROPERTIES
      OUTPUT_NAME PluginLayerAssignment CLEAN_DIRECT_OUTPUT 1)
  endif(UNIX)

endif(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)

set(CMAKE_AUTOMOC OFF)
