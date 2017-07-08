set(PluginDetectSpheres_VERSION_MAJOR 1)
set(PluginDetectSpheres_VERSION_MINOR 0)
set(PluginDetectSpheres_VERSION_PATCH 0)
set(PluginDetectSpheres_VERSION ${PluginDetectSpheres_VERSION_MAJOR}.${PluginDetectSpheres_VERSION_MINOR}.${PluginDetectSpheres_VERSION_PATCH})

set(CMAKE_AUTOMOC ON)

set(PluginDetectSpheres_HEADERS
  DetectSpheresParametersDialog.hh DetectSpheresThread.hh PluginDetectSpheres.hh)
set(PluginDetectSpheres_SOURCES
  DetectSpheresParametersDialog.cc DetectSpheresThread.cc PluginDetectSpheres.cc)

add_library(PluginDetectSpheres
  ${PluginDetectSpheres_SOURCES} ${PluginDetectSpheres_HEADERS})
target_link_libraries(PluginDetectSpheres Labelling_qt4 IRoCS)

# Install development headers
install(FILES ${PluginDetectSpheres_HEADERS}
  DESTINATION include/${PACKAGE}/plugins/libPluginDetectSpheres)

# Install libraries
if(WIN32)
  # On windows store DLLs alongside the binaries
  install(TARGETS PluginDetectSpheres
    LIBRARY DESTINATION share/${PACKAGE}/plugins NAMELINK_SKIP
    ARCHIVE DESTINATION lib)
else()
  install(TARGETS PluginDetectSpheres
    LIBRARY DESTINATION share/${PACKAGE}/plugins NAMELINK_SKIP
    ARCHIVE DESTINATION lib)
endif(WIN32)

# Explicitly add static library build if both static and shared libraries are
# requested
if(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)
  add_library(PluginDetectSpheres_static STATIC
    ${PluginDetectSpheres_SOURCES} ${PluginDetectSpheres_HEADERS})
  target_link_libraries(PluginDetectSpheres_static Labelling_qt4 IRoCS)
  install(TARGETS PluginDetectSpheres_static ARCHIVE DESTINATION lib)

  # On Linux rename static library to match shared library name
  if(UNIX)
    SET_TARGET_PROPERTIES(PluginDetectSpheres_static PROPERTIES
      OUTPUT_NAME PluginDetectSpheres CLEAN_DIRECT_OUTPUT 1)
  endif(UNIX)

endif(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)

set(CMAKE_AUTOMOC OFF)
