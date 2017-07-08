add_executable(labelEpidermis labelEpidermis.cc)
target_link_libraries(labelEpidermis LINK_PUBLIC IRoCS segmentation cmdline)

install(TARGETS labelEpidermis RUNTIME DESTINATION bin)
