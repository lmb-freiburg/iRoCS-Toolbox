add_executable(assignLayers assignLayers.cc)

if (BUILD_STATIC_TOOLS)
  target_link_libraries(assignLayers PUBLIC "-static")
  target_link_libraries(assignLayers
    LINK_PUBLIC IRoCS_static_tools segmentation_static_tools
    cmdline_static_tools)
else()
  target_link_libraries(assignLayers LINK_PUBLIC IRoCS segmentation cmdline)
endif()

install(TARGETS assignLayers RUNTIME DESTINATION bin)
