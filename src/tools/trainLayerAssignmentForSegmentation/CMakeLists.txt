add_executable(trainLayerAssignmentForSegmentation
  trainLayerAssignmentForSegmentation.cc)

if (BUILD_STATIC_TOOLS)
  target_link_libraries(trainLayerAssignmentForSegmentation PRIVATE "-static")
  target_link_libraries(trainLayerAssignmentForSegmentation
    PRIVATE IRoCS_static_tools segmentation_static_tools
    cmdline_static_tools)
elseif (BUILD_STATIC_LIBS)
  target_link_libraries(trainLayerAssignmentForSegmentation
    PRIVATE IRoCS_static segmentation_static cmdline_static)
elseif (BUILD_SHARED_LIBS)
  target_link_libraries(trainLayerAssignmentForSegmentation
    PRIVATE IRoCS segmentation cmdline)
endif()

install(TARGETS trainLayerAssignmentForSegmentation RUNTIME DESTINATION bin)
