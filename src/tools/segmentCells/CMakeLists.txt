add_executable(segmentCells segmentCells.cc)

if (BUILD_STATIC_TOOLS)
  target_link_libraries(segmentCells PUBLIC "-static")
  target_link_libraries(segmentCells
    LINK_PUBLIC IRoCS_static_tools segmentation_static_tools
    cmdline_static_tools)
else()
  target_link_libraries(segmentCells LINK_PUBLIC IRoCS segmentation cmdline)
endif()

install(TARGETS segmentCells RUNTIME DESTINATION bin)
