add_executable(assignLayers assignLayers.cc)
target_link_libraries(assignLayers LINK_PUBLIC IRoCS segmentation cmdline)

install(TARGETS assignLayers RUNTIME DESTINATION bin)
