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

add_library(PluginTrainEpidermisLabelling
  ${PluginTrainEpidermisLabelling_SOURCES} ${PluginTrainEpidermisLabelling_HEADERS})
target_link_libraries(PluginTrainEpidermisLabelling Labelling_qt4 IRoCS)

# Install development headers
install(FILES ${PluginTrainEpidermisLabelling_HEADERS}
  DESTINATION include/${PACKAGE}/plugins/libPluginTrainEpidermisLabelling)

# Install libraries
if(WIN32)
  # On windows store DLLs alongside the binaries
  install(TARGETS PluginTrainEpidermisLabelling
    LIBRARY DESTINATION share/${PACKAGE}/plugins NAMELINK_SKIP
    RUNTIME DESTINATION share/${PACKAGE}/plugins
    ARCHIVE DESTINATION lib)
else()
  install(TARGETS PluginTrainEpidermisLabelling
    LIBRARY DESTINATION share/${PACKAGE}/plugins NAMELINK_SKIP
    ARCHIVE DESTINATION lib)
endif(WIN32)

# Explicitly add static library build if both static and shared libraries are
# requested
if(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)
  add_library(PluginTrainEpidermisLabelling_static STATIC
    ${PluginTrainEpidermisLabelling_SOURCES} ${PluginTrainEpidermisLabelling_HEADERS})
  target_link_libraries(PluginTrainEpidermisLabelling_static Labelling_qt4 IRoCS)
  install(TARGETS PluginTrainEpidermisLabelling_static ARCHIVE DESTINATION lib)

  # On Linux rename static library to match shared library name
  if(UNIX)
    SET_TARGET_PROPERTIES(PluginTrainEpidermisLabelling_static PROPERTIES
      OUTPUT_NAME PluginTrainEpidermisLabelling CLEAN_DIRECT_OUTPUT 1)
  endif(UNIX)

endif(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)

set(CMAKE_AUTOMOC OFF)
