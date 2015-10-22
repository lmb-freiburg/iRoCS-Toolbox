lib_LTLIBRARIES = libsegmentation.la
segmentationincludedir = $(pkgincludedir)/libsegmentation

AM_CPPFLAGS = -I$(top_srcdir)/src $(BLITZ_CFLAGS) $(HDF5_CFLAGS) \
	$(FFTW_CFLAGS) $(GSL_CFLAGS) $(OPENCV_CFLAGS)
AM_CXXFLAGS = -Wno-long-long
AM_LDFLAGS = -no-undefined -version-info 1:0:0

libsegmentation_la_LIBADD = \
	$(top_builddir)/src/libArrayToolbox/libArrayToolbox.la \
	$(top_builddir)/src/libBlitzFFTW/libBlitzFFTW.la \
	$(top_builddir)/src/libBlitzHdf5/libBlitzHdf5.la \
	$(top_builddir)/src/libProgressReporter/libProgressReporter.la \
	$(top_builddir)/src/libBaseFunctions/libBaseFunctions.la \
	$(top_builddir)/src/lmbs2kit/liblmbs2kit.la \
	$(BLITZ_LIBS) $(HDF5_LIBS) $(FFTW_LIBS) $(GSL_LIBS) $(OPENCV_LIBS)

segmentationinclude_HEADERS = \
	EdgeFilter.hh EdgeFilter-inl.hh \
	kmeans.hh kmeans-inl.hh \
	segmentation_error.hh \
	segmentation.hh \
	NormalPDF.hh NormalPDF-inl.hh \
	helpers-inl.hh \
	ProfileFilter.hh ProfileFilter-inl.hh \
	ProfileSampler.hh ProfileSampler-inl.hh \
	SH_forward.hh SH_backward.hh SH_tools.hh \
	SegmentationModel.hh SegmentationModel-inl.hh \
	geodesic.hh geodesic.icc \
	SH_eval.hh \
	gvf.hh gvf-inl.hh


libsegmentation_la_SOURCES = \
	kmeans.cc \
	segmentation_error.cc \
	NormalPDF.cc \
	ProfileFilter.cc \
	ProfileSampler.cc \
	SegmentationModel.cc \
	geodesic.cc \
	SH_forward.cc SH_backward.cc SH_tools.cc \
	SH_eval.cc \
	gvf.cc

