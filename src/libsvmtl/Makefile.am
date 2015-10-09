lib_LTLIBRARIES = libsvmtl.la
svmtlincludedir = $(pkgincludedir)/libsvmtl
svt_checkincludedir=$(pkgincludedir)/libsvmtl/svt_check

AM_LDFLAGS = -version-info 1:0:0

libsvmtl_la_LIBADD = $(HDF5_LIBS)

svmtlinclude_HEADERS =					\
	AlgorithmLists.hh				\
	BasicCVAdapter.hh				\
	BasicCVAdapterTempl.hh				\
	BasicCVFactory.hh				\
	BasicFV.hh					\
	FVwithMultiClassCoefs.hh 			\
	BasicSVMAdapter.hh				\
	BasicSVMAdapterTempl.hh				\
	BasicSVMFactory.hh				\
	BasicSVMFactoryOneClass.hh			\
	CVAdapter.hh					\
	CVFactory.hh					\
	Cache.hh					\
	ClassificationStatistics.hh			\
	CrossValidator.hh				\
	CrossValidator.icc				\
	DefaultKernelList.hh				\
	DefaultMultiClassList.hh			\
	DefaultOneClassList.hh				\
	DefaultTwoClassList.hh				\
	DereferencingAccessor.hh			\
	DirectAccessor.hh				\
	GridAxis.hh					\
	GridSearch.hh					\
	GridSearch.icc					\
	GroupedTrainingData.hh				\
	GroupedTrainingData.icc				\
	HelpExtractor.hh				\
	Kernel.hh					\
	Kernel.icc					\
	Kernel_LINEAR.hh				\
	Kernel_MATRIX.hh				\
	Kernel_POLY.hh					\
	Kernel_RBF.hh					\
	Kernel_SCALE.hh					\
	Kernel_SCALE.icc				\
	Kernel_SIGMOID.hh				\
	Kernel_HISTINTERSECT.hh				\
	LoadSaveASCII.hh				\
	LoadSaveASCII.icc				\
	MC_SVM_Finder.hh				\
	Model.hh					\
	Model.icc					\
	Model_MC.hh					\
	Model_MC.icc					\
	Model_MC_OneVsOne.hh				\
	Model_MC_OneVsOne.icc				\
	Model_MC_OneVsRest.hh				\
	Model_MC_OneVsRest.icc				\
	MultiClassSVMOneVsOne.hh			\
	MultiClassSVMOneVsOne.icc			\
	MultiClassSVMOneVsRest.hh			\
	MultiClassSVMOneVsRest.icc			\
	ONE_CLASS_Q.hh					\
	OneClassSVMPlane.hh				\
	ParamInfo.hh					\
	PrettyOptionPrinter.hh				\
	ProgressReporter.hh				\
	ProgressReporterCerr.hh				\
	SVC_Q.hh					\
	SVMBase.hh					\
	SVMBase.icc					\
	SVMAdapter.hh					\
	SVMApplication.hh				\
	SVMApplication.icc				\
	SVMApplicationWithDefaults.hh			\
	SVMError.hh					\
	SVMFactory.hh					\
	SVMFactory.icc					\
	SVMFactoryOneClass.hh				\
	SVM_Problem.hh					\
	SVR_Q.hh					\
	SolutionInfo.hh					\
	Solver.hh					\
	Solver.icc					\
	Solver_NU.hh					\
	Solver_NU.icc					\
	SparseFV.hh					\
	StDataASCII.hh					\
	StDataASCII.icc					\
	StDataASCIIFile.hh				\
	StDataCmdLine.hh				\
	TList.hh					\
	TriangularMatrix.hh				\
	TwoClassSVM.hh					\
	TwoClassSVM.icc					\
	TwoClassSVMc.hh					\
	TwoClassSVMc.icc				\
	TwoClassSVMnu.hh				\
	TwoClassSVMnu.icc				\
	adjustUniqueIDs.hh				\
	svm_defines.hh					\
	GlobalIDFV.hh					\
	Kernel_DS_TRIA.hh				\
	Kernel_DS_RBF.hh				\
	Kernel_FILE.hh					\
	KFile_BINARY.hh					\
	KFile_MULTBINARY.hh				\
	Kernel_EDsqr.hh					\
	ModelOptimizer_linearNormal.hh			\
	ModelOptimizer_linearNormal.icc			\
	LoadSaveHdf5.hh					\
	LoadSaveHdf5.icc				\
	StDataHdf5.hh					\
	StDataHdf5.icc					\
	SvmH5Type.hh

libsvmtl_la_SOURCES =				\
	Cache.cc				\
	StDataASCII.cc				\
	StDataASCIIFile.cc			\
	StDataCmdLine.cc			\
	CrossValidator.cc			\
	GridAxis.cc				\
	ClassificationStatistics.cc		\
	PrettyOptionPrinter.cc			\
	StDataHdf5.cc

svt_checkinclude_HEADERS =                              \
	svt_check/MinimalFeatureVector.hh               \
	svt_check/MinimalFeatureVectorStream.hh         \
        svt_check/MinimalLoadSaveParameters.hh          \
	svt_check/MinimalStData.hh                      \
        svt_check/RequireFeatureVectorStream.hh         \
	svt_check/RequireFeatureVectorUniqueID.hh       \
        svt_check/RequireForwardIter.hh                 \
	svt_check/RequireHelpers.hh                     \
        svt_check/RequireKernel_k_function.hh           \
	svt_check/RequireLoadSaveParameters.hh          \
        svt_check/RequireMinimalTCModel.hh              \
	svt_check/RequireNameDescriptionParamInfos.hh   \
        svt_check/RequireSetProgressReporter.hh         \
        svt_check/RequireStDataFVArray.hh               \
        svt_check/RequireStData.hh                      \
        svt_check/RequireSVMBasicClassification.hh      \
        svt_check/RequireSVMBasicTraining.hh            \
        svt_check/RequireSVMForCrossValidation.hh       \
        svt_check/RequireSVMKernelUpdateCache.hh


EXTRA_DIST = doxy_definitions.hh
