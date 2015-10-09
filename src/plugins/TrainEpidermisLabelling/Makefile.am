lblplugindir = $(pkgdatadir)/plugins
lblplugin_LTLIBRARIES = libPluginTrainEpidermisLabelling.la
MOC = @QT4_MOC@

AM_CPPFLAGS = -I$(top_srcdir)/src $(BLITZ_CFLAGS) $(HDF5_CFLAGS) \
	$(FFTW_CFLAGS) $(GSL_CFLAGS) $(OPENCV_CFLAGS) $(QT4_CFLAGS)
AM_CXXFLAGS = -Wno-long-long
AM_LDFLAGS = -version-info 1:0:0

# Version bump to 2:0:0 pending

libPluginTrainEpidermisLabelling_la_LIBADD = \
	$(top_builddir)/src/libIRoCS/libIRoCS.la

MOC_GEN_SOURCES = \
	moc_TrainEpidermisLabellingTrainfileParametersItem.cc \
	moc_TrainEpidermisLabellingParametersDialog.cc \
	moc_TrainEpidermisLabellingThread.cc \
	moc_PluginTrainEpidermisLabelling.cc

libPluginTrainEpidermisLabelling_la_SOURCES = \
	TrainEpidermisLabellingTrainfileParametersItem.cc \
	TrainEpidermisLabellingParametersDialog.cc \
	TrainEpidermisLabellingThread.cc \
	PluginTrainEpidermisLabelling.cc \
	$(MOC_GEN_SOURCES)

moc_%.cc: %.hh
	$(MOC) $(AM_CPPFLAGS) $< -o $@

MOSTLYCLEANFILES = $(MOC_GEN_SOURCES) 
