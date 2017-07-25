add_executable(attachIRoCS attachIRoCS.cc)

if (BUILD_STATIC_TOOLS)
  target_link_libraries(attachIRoCS PRIVATE "-static")
  target_link_libraries(attachIRoCS
    PRIVATE IRoCS_static_tools segmentation_static_tools
    cmdline_static_tools)
elseif (BUILD_STATIC_LIBS)
  target_link_libraries(attachIRoCS
    PRIVATE IRoCS_static segmentation_static cmdline_static)
elseif (BUILD_SHARED_LIBS)
  target_link_libraries(attachIRoCS PRIVATE IRoCS segmentation cmdline)
endif()

install(TARGETS attachIRoCS RUNTIME DESTINATION bin)
