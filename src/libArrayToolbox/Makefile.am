lib_LTLIBRARIES = libArrayToolbox.la
arraytoolboxincludedir = $(pkgincludedir)/libArrayToolbox
arraytoolboxalgoincludedir = $(pkgincludedir)/libArrayToolbox/algo

AM_CPPFLAGS = -I$(top_srcdir)/src
AM_CXXFLAGS = -Wno-long-long
AM_LDFLAGS = -version-info @lt_major@:@lt_revision@:@lt_age@

arraytoolboxalgoinclude_HEADERS = \
	algo/DisjointSets.hh \
	algo/RootCoordinateTransform.hh \
	algo/ShellCoordinateTransform.hh \
	algo/helper.hh algo/helper.icc \
	algo/lBlitzRandomForest.hh \
	algo/lNumerical.hh algo/lNumerical.icc \
	algo/lParallel.hh algo/lParallel.icc \
	algo/lRandomForest.hh \
	algo/lRandomTree.hh \
	algo/ldiffusion.hh algo/ldiffusion.icc \
	algo/lmath.hh algo/lmath.icc \
	algo/lmorph.hh algo/lmorph.icc \
	algo/lrootShapeAnalysis.hh \
	algo/ltransform.hh algo/ltransform.icc

arraytoolboxinclude_HEADERS = \
	RuntimeError.hh \
	TypeTraits.hh TypeTraits.icc \
	Interpolator.hh Interpolator.icc \
	BoundaryTreatment.hh BoundaryTreatment.icc \
	TinyMatrixOperators.hh \
	Array.hh Array.icc \
	Neighborhood.hh Neighborhood.icc \
	ATBBasicTree.hh \
	Filter.hh Filter.icc \
	SeparableFilter.hh SeparableFilter.icc \
	SeparableCorrelationFilter.hh SeparableCorrelationFilter.icc \
	SeparableConvolutionFilter.hh SeparableConvolutionFilter.icc \
	GaussianFilter.hh GaussianFilter.icc \
	LaplacianOfGaussianFilter.hh LaplacianOfGaussianFilter.icc \
	CentralGradientFilter.hh CentralGradientFilter.icc \
	CentralHessianFilter.hh CentralHessianFilter.icc \
	CentralHessianUTFilter.hh CentralHessianUTFilter.icc \
	LaplacianFilter.hh LaplacianFilter.icc \
	MedianFilter.hh MedianFilter.icc \
	IsotropicMedianFilter.hh IsotropicMedianFilter.icc \
	IsotropicPercentileFilter.hh IsotropicPercentileFilter.icc \
	LocalSumFilter.hh LocalSumFilter.icc \
	DericheFilter_base.hh DericheFilter_base.icc \
	DericheFilter.hh DericheFilter.icc \
	FastCorrelationFilter.hh FastCorrelationFilter.icc \
	FastConvolutionFilter.hh FastConvolutionFilter.icc \
	FastNormalizedCorrelationFilter.hh FastNormalizedCorrelationFilter.icc \
	AnisotropicDiffusionFilter.hh AnisotropicDiffusionFilter.icc \
	SurfaceGeometry.hh \
	SparseVector.hh SparseVector.icc \
	SparseMatrix.hh SparseMatrix.icc \
	MarchingCubes.hh MarchingCubes.icc \
	Quaternion.hh \
	ATBTiming.hh \
	ATBLinAlg.hh ATBLinAlg.icc \
	ATBDataSynthesis.hh ATBDataSynthesis.icc \
	ATBMorphology.hh ATBMorphology.icc \
	ATBPolynomial.hh ATBPolynomial.icc \
	ATBGSLWrapper.hh ATBGSLWrapper.icc \
	ATBSpline.hh ATBSpline.icc \
	ATBPolyline.hh ATBPolyline.icc \
	ATBCoupledBSplineModel.hh ATBCoupledBSplineModel.icc \
	ATBNucleus.hh ATBNucleus.icc \
	iRoCS.hh iRoCS.icc \
	SphericalTensor.hh SphericalTensor.icc \
	HoughTransform.hh HoughTransform.icc \
	Random.hh Random.icc \
	LocalMaximumExtraction.hh LocalMaximumExtraction.icc \
	Normalization.hh Normalization.icc \
	STLFileWriter.hh \
	ATBThinPlateSpline.hh ATBThinPlateSpline.icc

libArrayToolbox_la_SOURCES = \
	algo/DisjointSet.cc \
	algo/RootCoordinateTransform.cc \
	algo/ShellCoordinateTransform.cc \
	algo/lRandomTree.cc \
	algo/lRandomForest.cc \
	algo/lBlitzRandomForest.cc \
	algo/ldiffusion.cc \
	algo/lmath.cc \
	algo/lmorph.cc \
	algo/lrootShapeAnalysis.cc \
	RuntimeError.cc \
	TypeTraits.cc \
	SurfaceGeometry.cc \
	MarchingCubes.cc \
	ATBTiming.cc \
	ATBLinAlg.cc \
	ATBDataSynthesis.cc \
	ATBGSLWrapper.cc \
	ATBSpline.cc \
	ATBCoupledBSplineModel.cc \
	ATBNucleus.cc \
	iRoCS.cc \
	SphericalTensor.cc \
	HoughTransform.cc \
	Random.cc \
	ATBThinPlateSpline.cc

