set(MultiChannelModel_VERSION_MAJOR 1)
set(MultiChannelModel_VERSION_MINOR 0)
set(MultiChannelModel_VERSION_PATCH 0)
set(MultiChannelModel_VERSION ${MultiChannelModel_VERSION_MAJOR}.${MultiChannelModel_VERSION_MINOR}.${MultiChannelModel_VERSION_PATCH})

set(MultiChannelModel_HEADERS
  Flags.hh Observable.hh
  MultichannelModel.hh MultichannelModelObserver.hh
  Channel.hh ChannelObserver.hh
  FloatChannel.hh FloatColormap.hh FloatColormapObserver.hh
  IndexedColormap.hh IndexedColormapObserver.hh)

set(MultiChannelModel_SOURCES
  Flags.cc Observable.cc
  MultichannelModel.cc MultichannelModelObserver.cc
  Channel.cc ChannelObserver.cc
  FloatChannel.cc FloatColormap.cc FloatColormapObserver.cc
  IndexedColormap.cc IndexedColormapObserver.cc)

if (BUILD_SHARED_LIBS OR BUILD_STATIC_LIBS)
  # Install development headers
  install(FILES ${MultiChannelModel_HEADERS}
    DESTINATION include/${PACKAGE}/libMultiChannelModel)
endif()

# Build shared library if requested
if (BUILD_SHARED_LIBS)
  add_library(MultiChannelModel SHARED
    ${MultiChannelModel_SOURCES} ${MultiChannelModel_HEADERS})
  set_target_properties(MultiChannelModel PROPERTIES
    VERSION ${MultiChannelModel_VERSION}
    SOVERSION ${MultiChannelModel_VERSION_MAJOR})
  target_link_libraries(MultiChannelModel PUBLIC ArrayToolbox)
  if(WIN32)
    target_compile_definitions(MultiChannelModel
    INTERFACE MultiChannelModel_USE_DLL)
  endif()
  install(TARGETS MultiChannelModel
    EXPORT iRoCS-ToolboxTargets
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include/${PACKAGE}/libMultiChannelModel)
endif()

# Build static library if requested
if(BUILD_STATIC_LIBS)
  add_library(MultiChannelModel_static STATIC
    ${MultiChannelModel_SOURCES} ${MultiChannelModel_HEADERS})
  target_link_libraries(MultiChannelModel_static PUBLIC ArrayToolbox_static)
  install(TARGETS MultiChannelModel_static
    EXPORT iRoCS-ToolboxTargets
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include/${PACKAGE}/libMultiChannelModel)

  # On Linux rename static library to match shared library name
  if(UNIX)
    SET_TARGET_PROPERTIES(MultiChannelModel_static PROPERTIES
      OUTPUT_NAME MultiChannelModel)
  endif()
endif()

# Explicitly add static library build for static binaries
if(BUILD_STATIC_TOOLS)
  add_library(MultiChannelModel_static_tools STATIC
    ${MultiChannelModel_SOURCES} ${MultiChannelModel_HEADERS})
  target_link_libraries(MultiChannelModel_static_tools
    PUBLIC ArrayToolbox_static_tools)
endif()
