set(segmentation_VERSION_MAJOR 2)
set(segmentation_VERSION_MINOR 0)
set(segmentation_VERSION_PATCH 0)
set(segmentation_VERSION ${segmentation_VERSION_MAJOR}.${segmentation_VERSION_MINOR}.${segmentation_VERSION_PATCH})

set(segmentation_HEADERS
  EdgeFilter.hh EdgeFilter-inl.hh kmeans.hh kmeans-inl.hh
  segmentation_error.hh segmentation.hh NormalPDF.hh NormalPDF-inl.hh
  helpers-inl.hh ProfileFilter.hh ProfileFilter-inl.hh ProfileSampler.hh
  ProfileSampler-inl.hh SH_forward.hh SH_backward.hh SH_tools.hh
  SegmentationModel.hh SegmentationModel-inl.hh geodesic.hh geodesic.icc
  SH_eval.hh gvf.hh gvf-inl.hh)

set(segmentation_SOURCES
  kmeans.cc segmentation_error.cc NormalPDF.cc ProfileFilter.cc
  ProfileSampler.cc SegmentationModel.cc geodesic.cc SH_forward.cc
  SH_backward.cc SH_tools.cc SH_eval.cc gvf.cc)

if (BUILD_SHARED_LIBS OR BUILD_STATIC_LIBS)
  # Install development headers
  install(FILES ${segmentation_HEADERS}
    DESTINATION include/${PACKAGE}/libsegmentation)
endif()

# Build shared library if requested
if (BUILD_SHARED_LIBS)
  add_library(segmentation SHARED
    ${segmentation_SOURCES} ${segmentation_HEADERS})
  set_target_properties(segmentation PROPERTIES
    VERSION ${segmentation_VERSION} SOVERSION ${segmentation_VERSION_MAJOR})
  target_link_libraries(segmentation PUBLIC ArrayToolbox lmbs2kit)
  install(TARGETS segmentation
    EXPORT iRoCS-ToolboxTargets
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include/${PACKAGE}/libsegmentation)
endif()

# Build static library if requested
if(BUILD_STATIC_LIBS)
  add_library(segmentation_static STATIC
    ${segmentation_SOURCES} ${segmentation_HEADERS})
  target_link_libraries(segmentation_static
    PUBLIC ArrayToolbox_static lmbs2kit_static)
  install(TARGETS segmentation_static
    EXPORT iRoCS-ToolboxTargets
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include/${PACKAGE}/libsegmentation)

  # On Linux rename static library to match shared library name
  if(UNIX)
    SET_TARGET_PROPERTIES(segmentation_static PROPERTIES
      OUTPUT_NAME segmentation)
  endif()
endif()

# Explicitly add static library build for static binaries
if(BUILD_STATIC_TOOLS)
  add_library(segmentation_static_tools STATIC
    ${segmentation_SOURCES} ${segmentation_HEADERS})
  target_link_libraries(segmentation_static_tools
    PUBLIC ArrayToolbox_static_tools lmbs2kit_static_tools)
endif()
