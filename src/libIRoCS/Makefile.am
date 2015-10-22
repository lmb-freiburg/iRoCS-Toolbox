lib_LTLIBRARIES = libIRoCS.la
irocsincludedir = $(pkgincludedir)/libIRoCS

AM_CPPFLAGS = -I$(top_srcdir)/src \
	$(GSL_CFLAGS) $(OPENCV_CFLAGS) $(BLITZ_CFLAGS) $(FFTW_CFLAGS) \
	$(HDF5_CFLAGS)
AM_CXXFLAGS = -Wno-long-long
AM_LDFLAGS = -no-undefined -version-info 2:0:0 -rdynamic

# Version bump to 3:0:0 pending (Interface change - removed liblabelling_qt4 dependencies)

libIRoCS_la_LIBADD = \
	$(top_builddir)/src/libArrayToolbox/libArrayToolbox.la \
	$(top_builddir)/src/libBlitzFFTW/libBlitzFFTW.la \
	$(top_builddir)/src/libBlitzHdf5/libBlitzHdf5.la \
	$(top_builddir)/src/libProgressReporter/libProgressReporter.la \
	$(top_builddir)/src/libBaseFunctions/libBaseFunctions.la \
	$(top_builddir)/src/libsvmtl/libsvmtl.la \
	$(BLITZ_LIBS) $(HDF5_LIBS) $(FFTW_LIBS) $(GSL_LIBS) $(OPENCV_LIBS)

irocsinclude_HEADERS = \
	iRoCSFeatures.hh iRoCSFeatures.icc \
	DetectNucleiWorker.hh \
	EpidermisLabellingWorker.hh \
	AttachIRoCSWorker.hh \
	LayerAssignmentWorker.hh \
	RootSegmentationWorker.hh \
	AttachIRoCSSCTToCellSegmentationWorker.hh \
	ComputeCellFeaturesWorker.hh \
	AssignLayersToCellSegmentationWorker.hh \
	TrainfileParameters.hh \
	TrainingParameters.hh \
	TrainDetectorWorker.hh \
	TrainEpidermisLabellingWorker.hh \
	TrainLayerAssignmentWorker.hh \
	DetectSpheresWorker.hh

libIRoCS_la_SOURCES = \
	iRoCSFeatures.cc \
	DetectNucleiWorker.cc \
	EpidermisLabellingWorker.cc \
	AttachIRoCSWorker.cc \
	LayerAssignmentWorker.cc \
	RootSegmentationWorker.cc \
	AttachIRoCSSCTToCellSegmentationWorker.cc \
	ComputeCellFeaturesWorker.cc \
	AssignLayersToCellSegmentationWorker.cc \
	TrainfileParameters.cc \
	TrainingParameters.cc \
	TrainDetectorWorker.cc \
	TrainEpidermisLabellingWorker.cc \
	TrainLayerAssignmentWorker.cc \
	DetectSpheresWorker.cc 
