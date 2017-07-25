set(IRoCS_VERSION_MAJOR 3)
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

if (BUILD_SHARED_LIBS OR BUILD_STATIC_LIBS)
  # Install development headers
  install(FILES ${IRoCS_HEADERS} DESTINATION include/${PACKAGE}/libIRoCS)
endif()

# Build shared library if requested
if (BUILD_SHARED_LIBS)
  add_library(IRoCS SHARED ${IRoCS_SOURCES} ${IRoCS_HEADERS})
  set_target_properties(IRoCS PROPERTIES
    VERSION ${IRoCS_VERSION} SOVERSION ${IRoCS_VERSION_MAJOR})
  target_link_libraries(IRoCS PUBLIC ArrayToolbox svmtl)
  install(TARGETS IRoCS
    EXPORT iRoCS-ToolboxTargets
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include/${PACKAGE}/libIRoCS)
endif()

# Build static library if requested
if(BUILD_STATIC_LIBS)
  add_library(IRoCS_static STATIC ${IRoCS_SOURCES} ${IRoCS_HEADERS})
  target_link_libraries(IRoCS_static PUBLIC ArrayToolbox_static svmtl_static)
  install(TARGETS IRoCS_static
    EXPORT iRoCS-ToolboxTargets
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include/${PACKAGE}/libIRoCS)

  # On Linux rename static library to match shared library name
  if(UNIX)
    SET_TARGET_PROPERTIES(IRoCS_static PROPERTIES OUTPUT_NAME IRoCS)
  endif()
endif()

# Explicitly add static library build for static binaries
if(BUILD_STATIC_TOOLS)
  add_library(IRoCS_static_tools STATIC ${IRoCS_SOURCES} ${IRoCS_HEADERS})
  target_link_libraries(IRoCS_static_tools
    PUBLIC ArrayToolbox_static_tools svmtl_static_tools)
endif()
