add_library(segmentation
  kmeans.cc segmentation_error.cc NormalPDF.cc ProfileFilter.cc ProfileSampler.cc
  SegmentationModel.cc geodesic.cc SH_forward.cc SH_backward.cc SH_tools.cc
  SH_eval.cc gvf.cc EdgeFilter.hh EdgeFilter-inl.hh kmeans.hh kmeans-inl.hh
  segmentation_error.hh segmentation.hh NormalPDF.hh NormalPDF-inl.hh
  helpers-inl.hh ProfileFilter.hh ProfileFilter-inl.hh ProfileSampler.hh
  ProfileSampler-inl.hh SH_forward.hh SH_backward.hh SH_tools.hh
  SegmentationModel.hh SegmentationModel-inl.hh geodesic.hh geodesic.icc
  SH_eval.hh gvf.hh gvf-inl.hh )

set(segmentation_VERSION_MAJOR 1)
set(segmentation_VERSION_MINOR 0)
set(segmentation_VERSION_PATCH 0)
set(segmentation_VERSION ${segmentation_VERSION_MAJOR}.${segmentation_VERSION_MINOR}.${segmentation_VERSION_PATCH})
set_target_properties(segmentation PROPERTIES VERSION ${segmentation_VERSION} SOVERSION ${segmentation_VERSION_MAJOR})

target_link_libraries(segmentation ArrayToolbox lmbs2kit )

# Install development headers
install(DIRECTORY . DESTINATION include/libsegmentation
  FILES_MATCHING PATTERN "*.hh" PATTERN "*.icc")

# Install libraries
if(WIN32)
  # On windows store DLLs alongside the binaries
  install(TARGETS segmentation
    LIBRARY DESTINATION bin
    ARCHIVE DESTINATION lib)
else()
  install(TARGETS segmentation
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
endif(WIN32)

# Explicitly add static library build if both static and shared libraries are requested
if(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)
  add_library(segmentation_static STATIC
    kmeans.cc segmentation_error.cc NormalPDF.cc ProfileFilter.cc ProfileSampler.cc
    SegmentationModel.cc geodesic.cc SH_forward.cc SH_backward.cc SH_tools.cc
    SH_eval.cc gvf.cc EdgeFilter.hh EdgeFilter-inl.hh kmeans.hh kmeans-inl.hh
    segmentation_error.hh segmentation.hh NormalPDF.hh NormalPDF-inl.hh
    helpers-inl.hh ProfileFilter.hh ProfileFilter-inl.hh ProfileSampler.hh
    ProfileSampler-inl.hh SH_forward.hh SH_backward.hh SH_tools.hh
    SegmentationModel.hh SegmentationModel-inl.hh geodesic.hh geodesic.icc
    SH_eval.hh gvf.hh gvf-inl.hh )
  target_link_libraries(segmentation_static ArrayToolbox lbs2kit )
  install(TARGETS segmentation_static ARCHIVE DESTINATION lib)

  # On Linux rename static library to match shared library name
  if(UNIX)
    SET_TARGET_PROPERTIES(segmentation_static PROPERTIES OUTPUT_NAME segmentation CLEAN_DIRECT_OUTPUT 1)
  endif(UNIX)

endif(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)
