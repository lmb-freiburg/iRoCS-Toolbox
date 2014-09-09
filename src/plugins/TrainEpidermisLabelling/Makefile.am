lblplugindir = $(pkgdatadir)/plugins
lblplugin_LTLIBRARIES = libPluginTrainEpidermisLabelling.la
MOC = @QT4_MOC@

AM_CPPFLAGS = -I$(top_srcdir)/src
AM_CXXFLAGS = -Wno-long-long

libPluginTrainEpidermisLabelling_la_LIBADD = \
	$(top_builddir)/src/libIRoCS/libIRoCS.la

MOC_GEN_SOURCES = \
	moc_TrainEpidermisLabellingTrainfileParametersItem.cc \
	moc_TrainEpidermisLabellingParametersDialog.cc \
	moc_PluginTrainEpidermisLabelling.cc

libPluginTrainEpidermisLabelling_la_SOURCES = \
	TrainEpidermisLabellingTrainfileParametersItem.cc \
	TrainEpidermisLabellingParametersDialog.cc \
	PluginTrainEpidermisLabelling.cc \
	$(MOC_GEN_SOURCES)

moc_%.cc: %.hh
	$(MOC) $(AM_CPPFLAGS) $< -o $@

MOSTLYCLEANFILES = $(MOC_GEN_SOURCES) 
