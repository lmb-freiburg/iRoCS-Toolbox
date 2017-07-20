set(PluginAttachIRoCSSCTToCellSegmentation_VERSION_MAJOR 1)
set(PluginAttachIRoCSSCTToCellSegmentation_VERSION_MINOR 0)
set(PluginAttachIRoCSSCTToCellSegmentation_VERSION_PATCH 0)
set(PluginAttachIRoCSSCTToCellSegmentation_VERSION ${PluginAttachIRoCSSCTToCellSegmentation_VERSION_MAJOR}.${PluginAttachIRoCSSCTToCellSegmentation_VERSION_MINOR}.${PluginAttachIRoCSSCTToCellSegmentation_VERSION_PATCH})

set(CMAKE_AUTOMOC ON)

set(PluginAttachIRoCSSCTToCellSegmentation_HEADERS
  AttachIRoCSSCTToCellSegmentationParametersDialog.hh
  AttachIRoCSSCTToCellSegmentationThread.hh
  PluginAttachIRoCSSCTToCellSegmentation.hh)
set(PluginAttachIRoCSSCTToCellSegmentation_SOURCES
  AttachIRoCSSCTToCellSegmentationParametersDialog.cc
  AttachIRoCSSCTToCellSegmentationThread.cc
  PluginAttachIRoCSSCTToCellSegmentation.cc)

add_library(PluginAttachIRoCSSCTToCellSegmentation
  ${PluginAttachIRoCSSCTToCellSegmentation_SOURCES}
  ${PluginAttachIRoCSSCTToCellSegmentation_HEADERS})
target_link_libraries(PluginAttachIRoCSSCTToCellSegmentation
  Labelling_qt4 IRoCS)

# Install development headers
install(FILES ${PluginAttachIRoCSSCTToCellSegmentation_HEADERS}
  DESTINATION
  include/${PACKAGE}/plugins/libPluginAttachIRoCSSCTToCellSegmentation)

# Install libraries
if(WIN32)
  # On windows store DLLs alongside the binaries
  install(TARGETS PluginAttachIRoCSSCTToCellSegmentation
    LIBRARY DESTINATION share/${PACKAGE}/plugins NAMELINK_SKIP
    RUNTIME DESTINATION share/${PACKAGE}/plugins
    ARCHIVE DESTINATION lib)
else()
  install(TARGETS PluginAttachIRoCSSCTToCellSegmentation
    LIBRARY DESTINATION share/${PACKAGE}/plugins NAMELINK_SKIP
    ARCHIVE DESTINATION lib)
endif(WIN32)

# Explicitly add static library build if both static and shared libraries are
# requested
if(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)
  add_library(PluginAttachIRoCSSCTToCellSegmentation_static STATIC
    ${PluginAttachIRoCSSCTToCellSegmentation_SOURCES}
    ${PluginAttachIRoCSSCTToCellSegmentation_HEADERS})
  target_link_libraries(PluginAttachIRoCSSCTToCellSegmentation_static
    Labelling_qt4 IRoCS)
  install(TARGETS PluginAttachIRoCSSCTToCellSegmentation_static
    ARCHIVE DESTINATION lib)

  # On Linux rename static library to match shared library name
  if(UNIX)
    SET_TARGET_PROPERTIES(PluginAttachIRoCSSCTToCellSegmentation_static
      PROPERTIES
      OUTPUT_NAME PluginAttachIRoCSSCTToCellSegmentation CLEAN_DIRECT_OUTPUT 1)
  endif(UNIX)

endif(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)

set(CMAKE_AUTOMOC OFF)
