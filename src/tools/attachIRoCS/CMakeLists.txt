add_executable(attachIRoCS attachIRoCS.cc)

if (BUILD_STATIC_TOOLS)
  target_link_libraries(attachIRoCS PUBLIC "-static")
  target_link_libraries(attachIRoCS
    LINK_PUBLIC IRoCS_static_tools segmentation_static_tools
    cmdline_static_tools)
else()
  target_link_libraries(attachIRoCS LINK_PUBLIC IRoCS segmentation cmdline)
endif()

install(TARGETS attachIRoCS RUNTIME DESTINATION bin)
