TESTS = \
	testSparseFV \
	testFeatureVectors \
	testKernels \
	testGroupedTrainingData \
	testTwoClassSVM \
	testTriangularMatrix \
	testMultiClassSVM  \
	testStDataASCII \
	testStDataASCIIFile \
	testStDataCmdLine \
	testAccessor \
	testSVMFactory \
	testSVMApplication \
	testCrossValidation \
	testHelpExtractor \
	testGridAxis \
	testGridSearch \
	testCVFactory \
	testClassificationStatistics \
	testPrettyOptionPrinter \
	testKernel_SCALE \
	testParamInfo \
	testTrainClassifyScaled \
	testStDataHdf5

check_PROGRAMS = $(TESTS)

AM_CPPFLAGS = -I$(top_srcdir)/src $(HDF5_CFLAGS) \
	-DTOP_SRC_DIR="\"$(shell (cd \$(top_srcdir); pwd))\""

LDADD = $(top_builddir)/src/libsvmtl/libsvmtl.la \
	$(HDF5_CPP_LIBS) $(HDF5_LIBS)

testFeatureVectors_SOURCES = testFeatureVectors.cc
testSparseFV_SOURCES = testSparseFV.cc
testKernels_SOURCES = testKernels.cc
testTwoClassSVM_SOURCES = testTwoClassSVM.cc
testTriangularMatrix_SOURCES = testTriangularMatrix.cc
testMultiClassSVM_SOURCES = testMultiClassSVM.cc
testSVMFactory_SOURCES = testSVMFactory.cc
testCrossValidation_SOURCES = testCrossValidation.cc
testAccessor_SOURCES = testAccessor.cc
testSVMApplication_SOURCES = testSVMApplication.cc
testGroupedTrainingData_SOURCES = testGroupedTrainingData.cc
testStDataASCII_SOURCES = testStDataASCII.cc
testStDataASCIIFile_SOURCES = testStDataASCIIFile.cc
testStDataCmdLine_SOURCES = testStDataCmdLine.cc
testHelpExtractor_SOURCES = testHelpExtractor.cc
testGridAxis_SOURCES = testGridAxis.cc
testGridSearch_SOURCES = testGridSearch.cc
testCVFactory_SOURCES = testCVFactory.cc
testClassificationStatistics_SOURCES = testClassificationStatistics.cc
testPrettyOptionPrinter_SOURCES = testPrettyOptionPrinter.cc
testKernel_SCALE_SOURCES = testKernel_SCALE.cc
testParamInfo_SOURCES = testParamInfo.cc
testTrainClassifyScaled_SOURCES = testTrainClassifyScaled.cc
testStDataHdf5_SOURCES = testStDataHdf5.cc

EXTRA_DIST = lmbunit.hh				\
	MyFeatureVector.hh			\
	MyKernel.hh                             \
	heart_scale.vec
