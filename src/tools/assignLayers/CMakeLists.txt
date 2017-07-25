add_executable(assignLayers assignLayers.cc)

if (BUILD_STATIC_TOOLS)
  target_link_libraries(assignLayers PRIVATE "-static")
  target_link_libraries(assignLayers
    PRIVATE IRoCS_static_tools segmentation_static_tools
    cmdline_static_tools)
elseif (BUILD_STATIC_LIBS)
  target_link_libraries(assignLayers
    PRIVATE IRoCS_static segmentation_static cmdline_static)
elseif (BUILD_SHARED_LIBS)
  target_link_libraries(assignLayers PRIVATE IRoCS segmentation cmdline)
endif()

install(TARGETS assignLayers RUNTIME DESTINATION bin)
