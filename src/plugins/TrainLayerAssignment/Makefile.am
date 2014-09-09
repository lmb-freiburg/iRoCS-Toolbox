lblplugindir = $(pkgdatadir)/plugins
lblplugin_LTLIBRARIES = libPluginTrainLayerAssignment.la
MOC = @QT4_MOC@

AM_CPPFLAGS = -I$(top_srcdir)/src
AM_CXXFLAGS = -Wno-long-long
AM_LDFLAGS = -version-info 1:0:0

libPluginTrainLayerAssignment_la_LIBADD = \
	$(top_builddir)/src/libIRoCS/libIRoCS.la

MOC_GEN_SOURCES = \
	moc_TrainLayerAssignmentTrainfileParametersItem.cc \
	moc_TrainLayerAssignmentParametersDialog.cc \
	moc_PluginTrainLayerAssignment.cc

libPluginTrainLayerAssignment_la_SOURCES = \
	TrainLayerAssignmentTrainfileParametersItem.cc \
	TrainLayerAssignmentParametersDialog.cc \
	PluginTrainLayerAssignment.cc \
	$(MOC_GEN_SOURCES)

moc_%.cc: %.hh
	$(MOC) $(AM_CPPFLAGS) $< -o $@

MOSTLYCLEANFILES = $(MOC_GEN_SOURCES) 
