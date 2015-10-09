lblplugindir = $(pkgdatadir)/plugins
lblplugin_LTLIBRARIES = libPluginTrainDetector.la
MOC = @QT4_MOC@

AM_CPPFLAGS = -I$(top_srcdir)/src $(BLITZ_CFLAGS) $(HDF5_CFLAGS) \
	$(FFTW_CFLAGS) $(GSL_CFLAGS) $(OPENCV_CFLAGS) $(QT4_CFLAGS)
AM_CXXFLAGS = -Wno-long-long
AM_LDFLAGS = -version-info 1:0:0

# Version bump to 2.0.0 pending

libPluginTrainDetector_la_LIBADD = $(top_builddir)/src/libIRoCS/libIRoCS.la

LIBADD = $(top_builddir)/src/libIRoCS/libIRoCS.la

MOC_GEN_SOURCES = \
	moc_TrainDetectorTrainfileParametersItem.cc \
	moc_TrainDetectorParametersDialog.cc \
	moc_TrainDetectorThread.cc \
	moc_PluginTrainDetector.cc

libPluginTrainDetector_la_SOURCES = \
	TrainDetectorTrainfileParametersItem.cc \
	TrainDetectorParametersDialog.cc \
	TrainDetectorThread.cc \
	PluginTrainDetector.cc \
	$(MOC_GEN_SOURCES)

moc_%.cc: %.hh
	$(MOC) $(AM_CPPFLAGS) $< -o $@

MOSTLYCLEANFILES = $(MOC_GEN_SOURCES) 
