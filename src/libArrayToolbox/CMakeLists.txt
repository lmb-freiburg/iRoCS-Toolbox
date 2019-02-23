set(ArrayToolbox_VERSION_MAJOR 3)
set(ArrayToolbox_VERSION_MINOR 0)
set(ArrayToolbox_VERSION_PATCH 0)
set(ArrayToolbox_VERSION ${ArrayToolbox_VERSION_MAJOR}.${ArrayToolbox_VERSION_MINOR}.${ArrayToolbox_VERSION_PATCH})

set(ArrayToolbox_algo_HEADERS
  algo/DisjointSets.hh algo/RootCoordinateTransform.hh
  algo/ShellCoordinateTransform.hh algo/helper.hh algo/helper.icc
  algo/lBlitzRandomForest.hh algo/lNumerical.hh algo/lNumerical.icc
  algo/lParallel.hh algo/lParallel.icc algo/lRandomForest.hh
  algo/lRandomTree.hh algo/ldiffusion.hh algo/ldiffusion.icc algo/lmath.hh
  algo/lmath.icc algo/lmorph.hh algo/lmorph.icc algo/lrootShapeAnalysis.hh
  algo/ltransform.hh algo/ltransform.icc)

set(ArrayToolbox_algo_SOURCES
  algo/DisjointSet.cc algo/RootCoordinateTransform.cc
  algo/ShellCoordinateTransform.cc algo/lRandomTree.cc algo/lRandomForest.cc
  algo/lBlitzRandomForest.cc algo/ldiffusion.cc algo/lmath.cc algo/lmorph.cc
  algo/lrootShapeAnalysis.cc algo/ltransform.cc)

set(ArrayToolbox_HEADERS
  RuntimeError.hh TypeTraits.hh TypeTraits.icc Interpolator.hh Interpolator.icc
  BoundaryTreatment.hh BoundaryTreatment.icc TinyMatrixOperators.hh
  Array.hh Array.icc Neighborhood.hh Neighborhood.icc ATBBasicTree.hh
  Filter.hh Filter.icc SeparableFilter.hh SeparableFilter.icc
  SeparableCorrelationFilter.hh SeparableCorrelationFilter.icc
  SeparableConvolutionFilter.hh SeparableConvolutionFilter.icc
  GaussianFilter.hh GaussianFilter.icc
  LaplacianOfGaussianFilter.hh LaplacianOfGaussianFilter.icc
  CentralGradientFilter.hh CentralGradientFilter.icc
  CentralHessianFilter.hh CentralHessianFilter.icc
  CentralHessianUTFilter.hh CentralHessianUTFilter.icc
  LaplacianFilter.hh LaplacianFilter.icc MedianFilter.hh MedianFilter.icc
  IsotropicMedianFilter.hh IsotropicMedianFilter.icc
  IsotropicPercentileFilter.hh IsotropicPercentileFilter.icc
  LocalSumFilter.hh LocalSumFilter.icc
  DericheFilter_base.hh DericheFilter_base.icc
  DericheFilter.hh DericheFilter.icc
  FastCorrelationFilter.hh FastCorrelationFilter.icc
  FastConvolutionFilter.hh FastConvolutionFilter.icc
  FastNormalizedCorrelationFilter.hh FastNormalizedCorrelationFilter.icc
  FastPhaseOnlyCorrelationFilter.hh FastPhaseOnlyCorrelationFilter.icc
  AnisotropicDiffusionFilter.hh AnisotropicDiffusionFilter.icc
  SurfaceGeometry.hh SparseVector.hh SparseVector.icc
  SparseMatrix.hh SparseMatrix.icc MarchingCubes.hh MarchingCubes.icc
  Quaternion.hh ATBTiming.hh ATBLinAlg.hh ATBLinAlg.icc
  ATBDataSynthesis.hh ATBDataSynthesis.icc ATBMorphology.hh ATBMorphology.icc
  ATBPolynomial.hh ATBPolynomial.icc ATBGSLWrapper.hh ATBGSLWrapper.icc
  ATBSpline.hh ATBSpline.icc ATBPolyline.hh ATBPolyline.icc
  ATBCoupledBSplineModel.hh ATBCoupledBSplineModel.icc
  ATBNucleus.hh ATBNucleus.icc iRoCS.hh iRoCS.icc
  SphericalTensor.hh SphericalTensor.icc
  HoughTransform.hh HoughTransform.icc Random.hh Random.icc
  LocalMaximumExtraction.hh LocalMaximumExtraction.icc
  Normalization.hh Normalization.icc STLFileWriter.hh
  ATBThinPlateSpline.hh ATBThinPlateSpline.icc
  DistanceTransform.hh DistanceTransform.icc)

set(ArrayToolbox_SOURCES
  RuntimeError.cc TypeTraits.cc SurfaceGeometry.cc
  MarchingCubes.cc ATBTiming.cc ATBLinAlg.cc ATBDataSynthesis.cc
  ATBGSLWrapper.cc ATBSpline.cc ATBCoupledBSplineModel.cc ATBNucleus.cc
  iRoCS.cc SphericalTensor.cc
  HoughTransform.cc Random.cc ATBThinPlateSpline.cc
  DistanceTransform.cc)

if (BUILD_SHARED_LIBS OR BUILD_STATIC_LIBS)
  # Install development headers
  install(FILES ${ArrayToolbox_HEADERS}
    DESTINATION include/${PACKAGE}/libArrayToolbox)
  install(FILES ${ArrayToolbox_algo_HEADERS}
    DESTINATION include/${PACKAGE}/libArrayToolbox/algo)
endif()

# Build shared library if requested
if (BUILD_SHARED_LIBS)
  add_library(ArrayToolbox SHARED
    ${ArrayToolbox_SOURCES} ${ArrayToolbox_algo_SOURCES}
    ${ArrayToolbox_HEADERS} ${ArrayToolbox_algo_HEADERS})
  set_target_properties(ArrayToolbox PROPERTIES
    VERSION ${ArrayToolbox_VERSION} SOVERSION ${ArrayToolbox_VERSION_MAJOR})
  target_include_directories(ArrayToolbox
    PUBLIC ${GSL_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})
  target_link_libraries(ArrayToolbox
    PUBLIC BlitzFFTW BlitzHdf5 ${GSL_LIBRARIES} ${OpenCV_LIBRARIES})
  if(WIN32)
    target_compile_definitions(ArrayToolbox INTERFACE ArrayToolbox_USE_DLL)
  endif()
  install(TARGETS ArrayToolbox
    EXPORT iRoCS-ToolboxTargets
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include/${PACKAGE}/libArrayToolbox)
endif()

# Build static library if requested
if(BUILD_STATIC_LIBS)
  add_library(ArrayToolbox_static STATIC
    ${ArrayToolbox_SOURCES} ${ArrayToolbox_algo_SOURCES}
    ${ArrayToolbox_HEADERS} ${ArrayToolbox_algo_HEADERS})
  target_include_directories(ArrayToolbox_static
    PUBLIC ${GSL_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})
  target_link_libraries(ArrayToolbox_static
    PUBLIC BlitzFFTW_static BlitzHdf5_static ${GSL_LIBRARIES}
    ${OpenCV_LIBRARIES})
  install(TARGETS ArrayToolbox_static
    EXPORT iRoCS-ToolboxTargets
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include/${PACKAGE}/libArrayToolbox)

  # On Linux rename static library to match shared library name
  if(UNIX)
    SET_TARGET_PROPERTIES(ArrayToolbox_static PROPERTIES
      OUTPUT_NAME ArrayToolbox)
  endif()
endif()

# Explicitly add static library build for static binaries
if(BUILD_STATIC_TOOLS)
  add_library(ArrayToolbox_static_tools STATIC
    ${ArrayToolbox_SOURCES} ${ArrayToolbox_algo_SOURCES}
    ${ArrayToolbox_HEADERS} ${ArrayToolbox_algo_HEADERS})
  target_include_directories(ArrayToolbox_static_tools
    PUBLIC ${GSL_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})
  target_link_libraries(ArrayToolbox_static_tools
    PUBLIC BlitzFFTW_static_tools BlitzHdf5_static_tools ${GSL_STATIC_LIBRARIES}
    ${OpenCV_STATIC_LIBRARIES})
  add_dependencies(ArrayToolbox_static_tools OpenCV_static)
endif()
