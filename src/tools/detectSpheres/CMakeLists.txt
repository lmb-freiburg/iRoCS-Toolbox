add_executable(detectSpheres detectSpheres.cc)
target_link_libraries(detectSpheres LINK_PUBLIC IRoCS segmentation cmdline)

install(TARGETS detectSpheres RUNTIME DESTINATION bin)
