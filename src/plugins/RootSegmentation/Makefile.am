lblplugindir = $(pkgdatadir)/plugins
lblplugin_LTLIBRARIES = libPluginRootSegmentation.la
MOC = @QT4_MOC@

AM_CPPFLAGS = -I$(top_srcdir)/src $(BLITZ_CFLAGS) $(HDF5_CFLAGS) \
	$(HDF5_CPP_CFLAGS) $(FFTW_CFLAGS) $(GSL_CFLAGS) $(OPENCV_CFLAGS) \
	$(QT4_CFLAGS)
AM_CXXFLAGS = -Wno-long-long
AM_LDFLAGS = -no-undefined -version-info 2:0:0

libPluginRootSegmentation_la_LIBADD = \
	$(top_builddir)/src/libIRoCS/libIRoCS.la \
	$(top_builddir)/src/liblabelling_qt4/libLabelling_qt4.la \
	$(top_builddir)/src/libsegmentation/libsegmentation.la \
	$(top_builddir)/src/libArrayToolbox/libArrayToolbox.la \
	$(top_builddir)/src/libBlitzFFTW/libBlitzFFTW.la \
	$(top_builddir)/src/libBlitzHdf5/libBlitzHdf5.la \
	$(top_builddir)/src/libBaseFunctions/libBaseFunctions.la \
	$(top_builddir)/src/lmbs2kit/liblmbs2kit.la \
	$(top_builddir)/src/libBlitzAnalyze/libBlitzAnalyze.la \
	$(top_builddir)/src/libProgressReporter_qt4/libProgressReporter_qt4.la \
	$(top_builddir)/src/libProgressReporter/libProgressReporter.la \
	$(top_builddir)/src/libcmdline/libcmdline.la \
	$(BLITZ_LIBS) $(HDF5_CPP_LIBS) $(HDF5_LIBS) $(FFTW_LIBS) $(GSL_LIBS) \
	$(OPENCV_LIBS) $(QT4_LIBS) $(GL_LIBS) $(GLU_LIBS)

noinst_HEADERS = \
	RootSegmentationParametersDialog.hh \
	RootSegmentationThread.hh \
	PluginRootSegmentation.hh
MOC_GEN_SOURCES = \
	moc_RootSegmentationParametersDialog.cc \
	moc_RootSegmentationThread.cc \
	moc_PluginRootSegmentation.cc

libPluginRootSegmentation_la_SOURCES = \
	RootSegmentationParametersDialog.cc \
	RootSegmentationThread.cc \
	PluginRootSegmentation.cc

nodist_libPluginRootSegmentation_la_SOURCES = \
	$(MOC_GEN_SOURCES)

moc_%.cc: %.hh
	$(MOC) $(AM_CPPFLAGS) $< -o $@

MOSTLYCLEANFILES = $(MOC_GEN_SOURCES)
