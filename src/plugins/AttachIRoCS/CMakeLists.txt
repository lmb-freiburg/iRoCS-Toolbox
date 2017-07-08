set(PluginAttachIRoCS_VERSION_MAJOR 1)
set(PluginAttachIRoCS_VERSION_MINOR 0)
set(PluginAttachIRoCS_VERSION_PATCH 0)
set(PluginAttachIRoCS_VERSION ${PluginAttachIRoCS_VERSION_MAJOR}.${PluginAttachIRoCS_VERSION_MINOR}.${PluginAttachIRoCS_VERSION_PATCH})

set(CMAKE_AUTOMOC ON)

set(PluginAttachIRoCS_HEADERS
  AttachIRoCSParametersDialog.hh AttachIRoCSThread.hh PluginAttachIRoCS.hh)
set(PluginAttachIRoCS_SOURCES
  AttachIRoCSParametersDialog.cc AttachIRoCSThread.cc PluginAttachIRoCS.cc)

add_library(PluginAttachIRoCS
  ${PluginAttachIRoCS_SOURCES} ${PluginAttachIRoCS_HEADERS})
target_link_libraries(PluginAttachIRoCS Labelling_qt4 IRoCS)

# Install development headers
install(FILES ${PluginAttachIRoCS_HEADERS}
  DESTINATION include/${PACKAGE}/plugins/libPluginAttachIRoCS)

# Install libraries
if(WIN32)
  # On windows store DLLs alongside the binaries
  install(TARGETS PluginAttachIRoCS
    LIBRARY DESTINATION share/${PACKAGE}/plugins NAMELINK_SKIP
    ARCHIVE DESTINATION lib)
else()
  install(TARGETS PluginAttachIRoCS
    LIBRARY DESTINATION share/${PACKAGE}/plugins NAMELINK_SKIP
    ARCHIVE DESTINATION lib)
endif(WIN32)

# Explicitly add static library build if both static and shared libraries are
# requested
if(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)
  add_library(PluginAttachIRoCS_static STATIC
    ${PluginAttachIRoCS_SOURCES} ${PluginAttachIRoCS_HEADERS})
  target_link_libraries(PluginAttachIRoCS_static Labelling_qt4 IRoCS)
  install(TARGETS PluginAttachIRoCS_static ARCHIVE DESTINATION lib)

  # On Linux rename static library to match shared library name
  if(UNIX)
    SET_TARGET_PROPERTIES(PluginAttachIRoCS_static PROPERTIES
      OUTPUT_NAME PluginAttachIRoCS CLEAN_DIRECT_OUTPUT 1)
  endif(UNIX)

endif(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)

set(CMAKE_AUTOMOC OFF)
