set(PluginAttachIRoCS_VERSION_MAJOR 1)
set(PluginAttachIRoCS_VERSION_MINOR 0)
set(PluginAttachIRoCS_VERSION_PATCH 0)
set(PluginAttachIRoCS_VERSION ${PluginAttachIRoCS_VERSION_MAJOR}.${PluginAttachIRoCS_VERSION_MINOR}.${PluginAttachIRoCS_VERSION_PATCH})

set(CMAKE_AUTOMOC ON)

set(PluginAttachIRoCS_HEADERS
  AttachIRoCSParametersDialog.hh AttachIRoCSThread.hh PluginAttachIRoCS.hh)
set(PluginAttachIRoCS_SOURCES
  AttachIRoCSParametersDialog.cc AttachIRoCSThread.cc PluginAttachIRoCS.cc)

add_library(PluginAttachIRoCS MODULE
  ${PluginAttachIRoCS_SOURCES} ${PluginAttachIRoCS_HEADERS})

target_link_libraries(PluginAttachIRoCS PRIVATE Labelling_qt4 IRoCS)

install(TARGETS PluginAttachIRoCS
  LIBRARY DESTINATION share/${PACKAGE}/plugins NAMELINK_SKIP
  RUNTIME DESTINATION share/${PACKAGE}/plugins)

set(CMAKE_AUTOMOC OFF)
