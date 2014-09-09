lblplugindir = $(pkgdatadir)/plugins
lblplugin_LTLIBRARIES = libPluginTrainDetector.la
MOC = @QT4_MOC@

AM_CPPFLAGS = -I$(top_srcdir)/src
AM_CXXFLAGS = -Wno-long-long

libPluginTrainDetector_la_LIBADD = $(top_builddir)/src/libIRoCS/libIRoCS.la

MOC_GEN_SOURCES = \
	moc_TrainDetectorTrainfileParametersItem.cc \
	moc_TrainDetectorParametersDialog.cc \
	moc_PluginTrainDetector.cc

libPluginTrainDetector_la_SOURCES = \
	TrainDetectorTrainfileParametersItem.cc \
	TrainDetectorParametersDialog.cc \
	PluginTrainDetector.cc \
	$(MOC_GEN_SOURCES)

moc_%.cc: %.hh
	$(MOC) $(AM_CPPFLAGS) $< -o $@

MOSTLYCLEANFILES = $(MOC_GEN_SOURCES) 
