add_executable(attachIRoCS attachIRoCS.cc)
target_link_libraries(attachIRoCS LINK_PUBLIC IRoCS segmentation cmdline)

install(TARGETS attachIRoCS RUNTIME DESTINATION bin)
