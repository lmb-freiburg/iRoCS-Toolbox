set(svmtl_VERSION_MAJOR 1)
set(svmtl_VERSION_MINOR 0)
set(svmtl_VERSION_PATCH 0)
set(svmtl_VERSION
  ${svmtl_VERSION_MAJOR}.${svmtl_VERSION_MINOR}.${svmtl_VERSION_PATCH})

set(svmtl_HEADERS
  AlgorithmLists.hh BasicCVAdapter.hh BasicCVAdapterTempl.hh BasicCVFactory.hh
  BasicFV.hh FVwithMultiClassCoefs.hh BasicSVMAdapter.hh
  BasicSVMAdapterTempl.hh BasicSVMFactory.hh BasicSVMFactoryOneClass.hh
  CVAdapter.hh CVFactory.hh Cache.hh ClassificationStatistics.hh
  CrossValidator.hh CrossValidator.icc DefaultKernelList.hh
  DefaultMultiClassList.hh DefaultOneClassList.hh DefaultTwoClassList.hh
  DereferencingAccessor.hh DirectAccessor.hh GridAxis.hh GridSearch.hh
  GridSearch.icc GroupedTrainingData.hh GroupedTrainingData.icc
  HelpExtractor.hh Kernel.hh Kernel.icc Kernel_LINEAR.hh Kernel_MATRIX.hh
  Kernel_POLY.hh Kernel_RBF.hh Kernel_SCALE.hh Kernel_SCALE.icc
  Kernel_SIGMOID.hh Kernel_HISTINTERSECT.hh LoadSaveASCII.hh LoadSaveASCII.icc
  MC_SVM_Finder.hh Model.hh Model.icc Model_MC.hh Model_MC.icc
  Model_MC_OneVsOne.hh Model_MC_OneVsOne.icc Model_MC_OneVsRest.hh
  Model_MC_OneVsRest.icc MultiClassSVMOneVsOne.hh MultiClassSVMOneVsOne.icc
  MultiClassSVMOneVsRest.hh MultiClassSVMOneVsRest.icc ONE_CLASS_Q.hh
  OneClassSVMPlane.hh ParamInfo.hh PrettyOptionPrinter.hh ProgressReporter.hh
  ProgressReporterCerr.hh SVC_Q.hh SVMBase.hh SVMBase.icc SVMAdapter.hh
  SVMApplication.hh SVMApplication.icc SVMApplicationWithDefaults.hh
  SVMError.hh SVMFactory.hh SVMFactory.icc SVMFactoryOneClass.hh SVM_Problem.hh
  SVR_Q.hh SolutionInfo.hh Solver.hh Solver.icc Solver_NU.hh Solver_NU.icc
  SparseFV.hh StDataASCII.hh StDataASCII.icc StDataASCIIFile.hh
  StDataCmdLine.hh TList.hh TriangularMatrix.hh TwoClassSVM.hh TwoClassSVM.icc
  TwoClassSVMc.hh TwoClassSVMc.icc TwoClassSVMnu.hh TwoClassSVMnu.icc
  adjustUniqueIDs.hh svm_defines.hh GlobalIDFV.hh Kernel_DS_TRIA.hh
  Kernel_DS_RBF.hh Kernel_FILE.hh KFile_BINARY.hh KFile_MULTBINARY.hh
  Kernel_EDsqr.hh ModelOptimizer_linearNormal.hh
  ModelOptimizer_linearNormal.icc LoadSaveHdf5.hh LoadSaveHdf5.icc
  StDataHdf5.hh StDataHdf5.icc SvmH5Type.hh)
set(svmtl_svt_check_HEADERS
  svt_check/MinimalFeatureVector.hh svt_check/MinimalFeatureVectorStream.hh
  svt_check/MinimalLoadSaveParameters.hh svt_check/MinimalStData.hh
  svt_check/RequireFeatureVectorStream.hh
  svt_check/RequireFeatureVectorUniqueID.hh svt_check/RequireForwardIter.hh
  svt_check/RequireHelpers.hh svt_check/RequireKernel_k_function.hh
  svt_check/RequireLoadSaveParameters.hh svt_check/RequireMinimalTCModel.hh
  svt_check/RequireNameDescriptionParamInfos.hh
  svt_check/RequireSetProgressReporter.hh svt_check/RequireStDataFVArray.hh
  svt_check/RequireStData.hh svt_check/RequireSVMBasicClassification.hh
  svt_check/RequireSVMBasicTraining.hh
  svt_check/RequireSVMForCrossValidation.hh
  svt_check/RequireSVMKernelUpdateCache.hh)
set(svmtl_noinst_HEADERS doxy_definitions.hh)
set(svmtl_SOURCES
  Cache.cc StDataASCII.cc StDataASCIIFile.cc StDataCmdLine.cc CrossValidator.cc
  GridAxis.cc ClassificationStatistics.cc PrettyOptionPrinter.cc StDataHdf5.cc)

add_library(svmtl
  ${svmtl_SOURCES} ${svmtl_HEADERS} ${svmtl_svt_check_HEADERS}
  ${svmtl_noinst_HEADERS})
set_target_properties(svmtl PROPERTIES
  VERSION ${svmtl_VERSION} SOVERSION ${svmtl_VERSION_MAJOR})
target_include_directories(svmtl PUBLIC
  ${HDF5_INCLUDE_DIRS} ${HDF5_CPP_INCLUDE_DIRS})
target_link_libraries(svmtl ${HDF5_CPP_LIBRARIES} ${HDF5_LIBRARIES})

# Install development headers
install(FILES ${svmtl_HEADERS} DESTINATION include/${PACKAGE}/libsvmtl)
install(FILES ${svmtl_svt_check_HEADERS}
  DESTINATION include/${PACKAGE}/libsvmtl/svt_check)

# Install libraries
if(WIN32)
  # On windows store DLLs alongside the binaries
  install(TARGETS svmtl
    LIBRARY DESTINATION bin
    ARCHIVE DESTINATION lib)
else()
  install(TARGETS svmtl
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
endif(WIN32)

# Explicitly add static library build if both static and shared libraries
# are requested
if(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)
  add_library(svmtl_static STATIC
    ${svmtl_SOURCES} ${svmtl_HEADERS} ${svmtl_svt_check_HEADERS}
    ${svmtl_noinst_HEADERS})
  target_include_directories(svmtl_static PUBLIC
    ${HDF5_INCLUDE_DIRS} ${HDF5_CPP_INCLUDE_DIRS})
  target_link_libraries(svmtl_static ${HDF5_CPP_LIBRARIES} ${HDF5_LIBRARIES})
  install(TARGETS svmtl_static ARCHIVE DESTINATION lib)

  # On Linux rename static library to match shared library name
  if(UNIX)
    SET_TARGET_PROPERTIES(svmtl_static PROPERTIES
      OUTPUT_NAME svmtl CLEAN_DIRECT_OUTPUT 1)
  endif(UNIX)

endif(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)
