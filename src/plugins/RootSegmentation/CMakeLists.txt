set(PluginRootSegmentation_VERSION_MAJOR 1)
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

add_library(PluginRootSegmentation
  ${PluginRootSegmentation_SOURCES} ${PluginRootSegmentation_HEADERS})
target_link_libraries(PluginRootSegmentation Labelling_qt4 IRoCS)

# Install development headers
install(FILES ${PluginRootSegmentation_HEADERS}
  DESTINATION include/${PACKAGE}/plugins/libPluginRootSegmentation)

# Install libraries
if(WIN32)
  # On windows store DLLs alongside the binaries
  install(TARGETS PluginRootSegmentation
    LIBRARY DESTINATION share/${PACKAGE}/plugins NAMELINK_SKIP
    ARCHIVE DESTINATION lib)
else()
  install(TARGETS PluginRootSegmentation
    LIBRARY DESTINATION share/${PACKAGE}/plugins NAMELINK_SKIP
    ARCHIVE DESTINATION lib)
endif(WIN32)

# Explicitly add static library build if both static and shared libraries are
# requested
if(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)
  add_library(PluginRootSegmentation_static STATIC
    ${PluginRootSegmentation_SOURCES} ${PluginRootSegmentation_HEADERS})
  target_link_libraries(PluginRootSegmentation_static Labelling_qt4 IRoCS)
  install(TARGETS PluginRootSegmentation_static ARCHIVE DESTINATION lib)

  # On Linux rename static library to match shared library name
  if(UNIX)
    SET_TARGET_PROPERTIES(PluginRootSegmentation_static PROPERTIES
      OUTPUT_NAME PluginRootSegmentation CLEAN_DIRECT_OUTPUT 1)
  endif(UNIX)

endif(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)

set(CMAKE_AUTOMOC OFF)
