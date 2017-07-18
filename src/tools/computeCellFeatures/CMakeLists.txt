add_executable(computeCellFeatures computeCellFeatures.cc)

if (BUILD_STATIC_TOOLS)
  target_link_libraries(computeCellFeatures PUBLIC "-static")
  target_link_libraries(computeCellFeatures
    LINK_PUBLIC IRoCS_static_tools segmentation_static_tools
    cmdline_static_tools)
else()
  target_link_libraries(computeCellFeatures
    LINK_PUBLIC IRoCS segmentation cmdline)
endif()

install(TARGETS computeCellFeatures RUNTIME DESTINATION bin)
