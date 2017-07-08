add_executable(segmentCells segmentCells.cc)
target_link_libraries(segmentCells LINK_PUBLIC IRoCS segmentation cmdline)

install(TARGETS segmentCells RUNTIME DESTINATION bin)
