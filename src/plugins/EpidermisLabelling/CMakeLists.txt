set(PluginEpidermisLabelling_VERSION_MAJOR 1)
set(PluginEpidermisLabelling_VERSION_MINOR 0)
set(PluginEpidermisLabelling_VERSION_PATCH 0)
set(PluginEpidermisLabelling_VERSION ${PluginEpidermisLabelling_VERSION_MAJOR}.${PluginEpidermisLabelling_VERSION_MINOR}.${PluginEpidermisLabelling_VERSION_PATCH})

set(CMAKE_AUTOMOC ON)

set(PluginEpidermisLabelling_HEADERS
  EpidermisLabellingParametersDialog.hh EpidermisLabellingThread.hh
  PluginEpidermisLabelling.hh)
set(PluginEpidermisLabelling_SOURCES
  EpidermisLabellingParametersDialog.cc EpidermisLabellingThread.cc
  PluginEpidermisLabelling.cc)

add_library(PluginEpidermisLabelling
  ${PluginEpidermisLabelling_SOURCES} ${PluginEpidermisLabelling_HEADERS})
target_link_libraries(PluginEpidermisLabelling Labelling_qt4 IRoCS)

# Install development headers
install(FILES ${PluginEpidermisLabelling_HEADERS}
  DESTINATION include/${PACKAGE}/plugins/libPluginEpidermisLabelling)

# Install libraries
if(WIN32)
  # On windows store DLLs alongside the binaries
  install(TARGETS PluginEpidermisLabelling
    LIBRARY DESTINATION share/${PACKAGE}/plugins NAMELINK_SKIP
    ARCHIVE DESTINATION lib)
else()
  install(TARGETS PluginEpidermisLabelling
    LIBRARY DESTINATION share/${PACKAGE}/plugins NAMELINK_SKIP
    ARCHIVE DESTINATION lib)
endif(WIN32)

# Explicitly add static library build if both static and shared libraries are
# requested
if(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)
  add_library(PluginEpidermisLabelling_static STATIC
    ${PluginEpidermisLabelling_SOURCES} ${PluginEpidermisLabelling_HEADERS})
  target_link_libraries(PluginEpidermisLabelling_static Labelling_qt4 IRoCS)
  install(TARGETS PluginEpidermisLabelling_static ARCHIVE DESTINATION lib)

  # On Linux rename static library to match shared library name
  if(UNIX)
    SET_TARGET_PROPERTIES(PluginEpidermisLabelling_static PROPERTIES
      OUTPUT_NAME PluginEpidermisLabelling CLEAN_DIRECT_OUTPUT 1)
  endif(UNIX)

endif(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)

set(CMAKE_AUTOMOC OFF)
