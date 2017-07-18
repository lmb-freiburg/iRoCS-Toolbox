add_executable(labelEpidermis labelEpidermis.cc)

if (BUILD_STATIC_TOOLS)
  target_link_libraries(labelEpidermis PUBLIC "-static")
  target_link_libraries(labelEpidermis
    LINK_PUBLIC IRoCS_static_tools segmentation_static_tools
    cmdline_static_tools)
else()
  target_link_libraries(labelEpidermis LINK_PUBLIC IRoCS segmentation cmdline)
endif()

install(TARGETS labelEpidermis RUNTIME DESTINATION bin)
