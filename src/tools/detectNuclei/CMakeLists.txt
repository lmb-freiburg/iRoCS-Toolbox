add_executable(detectNuclei detectNuclei.cc)

if (BUILD_STATIC_TOOLS)
  target_link_libraries(detectNuclei PUBLIC "-static")
  target_link_libraries(detectNuclei
    LINK_PUBLIC IRoCS_static_tools segmentation_static_tools
    cmdline_static_tools)
else()
  target_link_libraries(detectNuclei
    LINK_PUBLIC IRoCS segmentation cmdline)
endif()

install(TARGETS detectNuclei RUNTIME DESTINATION bin)
