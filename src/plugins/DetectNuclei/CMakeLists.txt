set(PluginDetectNuclei_VERSION_MAJOR 1)
set(PluginDetectNuclei_VERSION_MINOR 0)
set(PluginDetectNuclei_VERSION_PATCH 0)
set(PluginDetectNuclei_VERSION ${PluginDetectNuclei_VERSION_MAJOR}.${PluginDetectNuclei_VERSION_MINOR}.${PluginDetectNuclei_VERSION_PATCH})

set(CMAKE_AUTOMOC ON)

set(PluginDetectNuclei_HEADERS
  DetectNucleiParametersDialog.hh DetectNucleiThread.hh PluginDetectNuclei.hh)
set(PluginDetectNuclei_SOURCES
  DetectNucleiParametersDialog.cc DetectNucleiThread.cc PluginDetectNuclei.cc)

add_library(PluginDetectNuclei
  ${PluginDetectNuclei_SOURCES} ${PluginDetectNuclei_HEADERS})
target_link_libraries(PluginDetectNuclei Labelling_qt4 IRoCS)

# Install development headers
install(FILES ${PluginDetectNuclei_HEADERS}
  DESTINATION include/${PACKAGE}/plugins/libPluginDetectNuclei)

# Install libraries
if(WIN32)
  # On windows store DLLs alongside the binaries
  install(TARGETS PluginDetectNuclei
    LIBRARY DESTINATION share/${PACKAGE}/plugins NAMELINK_SKIP
    ARCHIVE DESTINATION lib)
else()
  install(TARGETS PluginDetectNuclei
    LIBRARY DESTINATION share/${PACKAGE}/plugins NAMELINK_SKIP
    ARCHIVE DESTINATION lib)
endif(WIN32)

# Explicitly add static library build if both static and shared libraries are
# requested
if(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)
  add_library(PluginDetectNuclei_static STATIC
    ${PluginDetectNuclei_SOURCES} ${PluginDetectNuclei_HEADERS})
  target_link_libraries(PluginDetectNuclei_static Labelling_qt4 IRoCS)
  install(TARGETS PluginDetectNuclei_static ARCHIVE DESTINATION lib)

  # On Linux rename static library to match shared library name
  if(UNIX)
    SET_TARGET_PROPERTIES(PluginDetectNuclei_static PROPERTIES
      OUTPUT_NAME PluginDetectNuclei CLEAN_DIRECT_OUTPUT 1)
  endif(UNIX)

endif(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)

set(CMAKE_AUTOMOC OFF)
