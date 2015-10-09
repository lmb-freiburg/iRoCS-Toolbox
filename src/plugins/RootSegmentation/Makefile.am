lblplugindir = $(pkgdatadir)/plugins
lblplugin_LTLIBRARIES = libPluginRootSegmentation.la
MOC = @QT4_MOC@

AM_CPPFLAGS = -I$(top_srcdir)/src $(BLITZ_CFLAGS) $(HDF5_CFLAGS) \
	$(FFTW_CFLAGS) $(GSL_CFLAGS) $(OPENCV_CFLAGS) $(QT4_CFLAGS)
AM_CXXFLAGS = -Wno-long-long
AM_LDFLAGS = -version-info 1:0:0

libPluginRootSegmentation_la_LIBADD = $(top_builddir)/src/libIRoCS/libIRoCS.la

LIBADD = $(top_builddir)/src/libIRoCS/libIRoCS.la

MOC_GEN_SOURCES = \
	moc_RootSegmentationParametersDialog.cc \
	moc_RootSegmentationThread.cc \
	moc_PluginRootSegmentation.cc

libPluginRootSegmentation_la_SOURCES = \
	RootSegmentationParametersDialog.cc \
	RootSegmentationThread.cc \
	PluginRootSegmentation.cc \
	$(MOC_GEN_SOURCES)

moc_%.cc: %.hh
	$(MOC) $(AM_CPPFLAGS) $< -o $@

MOSTLYCLEANFILES = $(MOC_GEN_SOURCES) 
