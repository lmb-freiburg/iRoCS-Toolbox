lblplugindir = $(pkgdatadir)/plugins
lblplugin_LTLIBRARIES = libPluginTrainLayerAssignment.la
MOC = @QT4_MOC@

AM_CPPFLAGS = -I$(top_srcdir)/src $(BLITZ_CFLAGS) $(HDF5_CFLAGS) \
	$(FFTW_CFLAGS) $(GSL_CFLAGS) $(OPENCV_CFLAGS) $(QT4_CFLAGS)
AM_CXXFLAGS = -Wno-long-long
AM_LDFLAGS = -version-info 1:0:0

# Version bump to 2:0:0 pending

libPluginTrainLayerAssignment_la_LIBADD = \
	$(top_builddir)/src/libIRoCS/libIRoCS.la

MOC_GEN_SOURCES = \
	moc_TrainLayerAssignmentTrainfileParametersItem.cc \
	moc_TrainLayerAssignmentParametersDialog.cc \
	moc_TrainLayerAssignmentThread.cc \
	moc_PluginTrainLayerAssignment.cc

libPluginTrainLayerAssignment_la_SOURCES = \
	TrainLayerAssignmentTrainfileParametersItem.cc \
	TrainLayerAssignmentParametersDialog.cc \
	TrainLayerAssignmentThread.cc \
	PluginTrainLayerAssignment.cc \
	$(MOC_GEN_SOURCES)

moc_%.cc: %.hh
	$(MOC) $(AM_CPPFLAGS) $< -o $@

MOSTLYCLEANFILES = $(MOC_GEN_SOURCES) 
