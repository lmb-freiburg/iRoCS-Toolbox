add_executable(computeCellFeatures computeCellFeatures.cc)
target_link_libraries(computeCellFeatures
  LINK_PUBLIC IRoCS segmentation cmdline)

install(TARGETS computeCellFeatures RUNTIME DESTINATION bin)
