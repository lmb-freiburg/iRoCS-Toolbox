add_executable(attachIRoCSToCellSegmentation attachIRoCSToCellSegmentation.cc)

if (BUILD_STATIC_TOOLS)
  target_link_libraries(attachIRoCSToCellSegmentation PUBLIC "-static")
  target_link_libraries(attachIRoCSToCellSegmentation
    LINK_PUBLIC IRoCS_static_tools segmentation_static_tools
    cmdline_static_tools)
else()
  target_link_libraries(attachIRoCSToCellSegmentation
    LINK_PUBLIC IRoCS segmentation cmdline)
endif()

install(TARGETS attachIRoCSToCellSegmentation RUNTIME DESTINATION bin)
