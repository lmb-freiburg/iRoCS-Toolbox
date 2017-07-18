add_executable(detectSpheres detectSpheres.cc)

if (BUILD_STATIC_TOOLS)
  target_link_libraries(detectSpheres PUBLIC "-static")
  target_link_libraries(detectSpheres
    LINK_PUBLIC IRoCS_static_tools segmentation_static_tools
    cmdline_static_tools)
else()
  target_link_libraries(detectSpheres
    LINK_PUBLIC IRoCS segmentation cmdline)
endif()

install(TARGETS detectSpheres RUNTIME DESTINATION bin)
