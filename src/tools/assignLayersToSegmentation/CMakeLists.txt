add_executable(assignLayersToSegmentation assignLayersToSegmentation.cc)

if (BUILD_STATIC_TOOLS)
  target_link_libraries(assignLayersToSegmentation PUBLIC "-static")
  target_link_libraries(assignLayersToSegmentation
    LINK_PUBLIC IRoCS_static_tools segmentation_static_tools
    cmdline_static_tools)
else()
  target_link_libraries(assignLayersToSegmentation
    LINK_PUBLIC IRoCS segmentation cmdline)
endif()

install(TARGETS assignLayersToSegmentation RUNTIME DESTINATION bin)
