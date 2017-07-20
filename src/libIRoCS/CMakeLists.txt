set(IRoCS_VERSION_MAJOR 2) # Version bump to 3.0.0 pending
set(IRoCS_VERSION_MINOR 0)
set(IRoCS_VERSION_PATCH 0)
set(IRoCS_VERSION ${IRoCS_VERSION_MAJOR}.${IRoCS_VERSION_MINOR}.${IRoCS_VERSION_PATCH})

set(IRoCS_HEADERS
  iRoCSFeatures.hh iRoCSFeatures.icc DetectNucleiWorker.hh
  EpidermisLabellingWorker.hh AttachIRoCSWorker.hh LayerAssignmentWorker.hh
  RootSegmentationWorker.hh AttachIRoCSSCTToCellSegmentationWorker.hh
  ComputeCellFeaturesWorker.hh AssignLayersToCellSegmentationWorker.hh
  TrainfileParameters.hh TrainingParameters.hh TrainDetectorWorker.hh
  TrainEpidermisLabellingWorker.hh TrainLayerAssignmentWorker.hh
  DetectSpheresWorker.hh)

set(IRoCS_SOURCES
  iRoCSFeatures.cc DetectNucleiWorker.cc EpidermisLabellingWorker.cc
  AttachIRoCSWorker.cc LayerAssignmentWorker.cc RootSegmentationWorker.cc
  AttachIRoCSSCTToCellSegmentationWorker.cc ComputeCellFeaturesWorker.cc
  AssignLayersToCellSegmentationWorker.cc TrainfileParameters.cc
  TrainingParameters.cc TrainDetectorWorker.cc TrainEpidermisLabellingWorker.cc
  TrainLayerAssignmentWorker.cc DetectSpheresWorker.cc)

add_library(IRoCS ${IRoCS_SOURCES} ${IRoCS_HEADERS})
set_target_properties(IRoCS PROPERTIES
  VERSION ${IRoCS_VERSION} SOVERSION ${IRoCS_VERSION_MAJOR})
target_link_libraries(IRoCS ArrayToolbox svmtl)

# Install development headers
install(FILES ${IRoCS_HEADERS} DESTINATION include/${PACKAGE}/libIRoCS)

# Install libraries
if(WIN32)
  # On windows store DLLs alongside the binaries
  install(TARGETS IRoCS
    LIBRARY DESTINATION bin
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib)
else()
  install(TARGETS IRoCS
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
endif(WIN32)

# Explicitly add static library build if both static and shared libraries
# are requested
if(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)
  add_library(IRoCS_static STATIC ${IRoCS_SOURCES} ${IRoCS_HEADERS})
  target_link_libraries(IRoCS_static ArrayToolbox svmtl)
  install(TARGETS IRoCS_static ARCHIVE DESTINATION lib)

  # On Linux rename static library to match shared library name
  if(UNIX)
    SET_TARGET_PROPERTIES(IRoCS_static PROPERTIES OUTPUT_NAME IRoCS)
  endif()
endif()

# Explicitly add static library build for static binaries
if(BUILD_STATIC_TOOLS)
  add_library(IRoCS_static_tools STATIC ${IRoCS_SOURCES} ${IRoCS_HEADERS})
  target_link_libraries(IRoCS_static_tools
    ArrayToolbox_static_tools svmtl_static_tools)
endif()
