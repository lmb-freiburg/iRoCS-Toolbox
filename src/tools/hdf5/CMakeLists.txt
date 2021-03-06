macro(buildTool target)
  add_executable(${target} ${target}.cc)
  set_target_properties(${target} PROPERTIES COMPILE_FLAGS -std=c++11)
  if (BUILD_STATIC_TOOLS)
    target_link_libraries(${target} PRIVATE "-static")
    target_link_libraries(${target}
      PRIVATE ArrayToolbox_static_tools cmdline_static_tools)
  elseif (BUILD_STATIC_LIBS)
    target_link_libraries(${target} PRIVATE ArrayToolbox_static cmdline_static)
  elseif (BUILD_SHARED_LIBS)
    target_link_libraries(${target} PRIVATE ArrayToolbox cmdline)
  endif ()
  install(TARGETS ${target} RUNTIME DESTINATION bin)
endmacro()

buildTool(h5cp)
buildTool(h5rm)
buildTool(existsDataset)
buildTool(existsAttribute)
buildTool(writeAttribute)
buildTool(setElementSizeUm)
buildTool(setTransformationMatrix)
buildTool(cropDataset)
buildTool(cropAnnotationChannel)
buildTool(scaleDataset)
buildTool(rotateDataset)
