add_executable(detectNuclei detectNuclei.cc)
target_link_libraries(detectNuclei LINK_PUBLIC IRoCS segmentation cmdline)

install(TARGETS detectNuclei RUNTIME DESTINATION bin)
