lblplugindir = $(pkgdatadir)/plugins
lblplugin_LTLIBRARIES = libPluginTrainEpidermisLabelling.la
MOC = @QT4_MOC@

AM_CPPFLAGS = -I$(top_srcdir)/src $(BLITZ_CFLAGS) $(HDF5_CFLAGS) \
	$(FFTW_CFLAGS) $(GSL_CFLAGS) $(OPENCV_CFLAGS) $(QT4_CFLAGS)
AM_CXXFLAGS = -Wno-long-long
AM_LDFLAGS = -no-undefined -version-info 1:0:0

# Version bump to 2:0:0 pending

libPluginTrainEpidermisLabelling_la_LIBADD = \
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
	$(BLITZ_LIBS) $(HDF5_LIBS) $(FFTW_LIBS) $(GSL_LIBS) $(OPENCV_LIBS) \
	$(QT4_LIBS) $(GL_LIBS) $(GLU_LIBS)

noinst_HEADERS = \
	TrainEpidermisLabellingTrainfileParametersItem.hh \
	TrainEpidermisLabellingParametersDialog.hh \
	TrainEpidermisLabellingThread.hh \
	PluginTrainEpidermisLabelling.hh

MOC_GEN_SOURCES = \
	moc_TrainEpidermisLabellingTrainfileParametersItem.cc \
	moc_TrainEpidermisLabellingParametersDialog.cc \
	moc_TrainEpidermisLabellingThread.cc \
	moc_PluginTrainEpidermisLabelling.cc

libPluginTrainEpidermisLabelling_la_SOURCES = \
	TrainEpidermisLabellingTrainfileParametersItem.cc \
	TrainEpidermisLabellingParametersDialog.cc \
	TrainEpidermisLabellingThread.cc \
	PluginTrainEpidermisLabelling.cc

nodist_libPluginTrainEpidermisLabelling_la_SOURCES = \
	$(MOC_GEN_SOURCES)

moc_%.cc: %.hh
	$(MOC) $(AM_CPPFLAGS) $< -o $@

MOSTLYCLEANFILES = $(MOC_GEN_SOURCES) 
