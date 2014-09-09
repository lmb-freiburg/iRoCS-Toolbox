lib_LTLIBRARIES = libsegmentation.la
segmentationincludedir = $(pkgincludedir)/libsegmentation

AM_CXXFLAGS = -Wno-long-long
AM_CPPFLAGS = -I$(top_srcdir)/src
AM_LDFLAGS = -version-info 1:0:0

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

