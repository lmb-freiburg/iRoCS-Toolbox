lblplugindir = $(pkgdatadir)/plugins
lblplugin_LTLIBRARIES = libPluginEpidermisLabelling.la
MOC = @QT4_MOC@

AM_CPPFLAGS = -I$(top_srcdir)/src
AM_CXXFLAGS = -Wno-long-long
AM_LDFLAGS = -version-info 1:0:0

libPluginEpidermisLabelling_la_LIBADD = \
	$(top_builddir)/src/libIRoCS/libIRoCS.la

MOC_GEN_SOURCES = \
	moc_EpidermisLabellingParametersDialog.cc \
	moc_EpidermisLabellingThread.cc \
	moc_PluginEpidermisLabelling.cc

libPluginEpidermisLabelling_la_SOURCES = \
	EpidermisLabellingParametersDialog.cc \
	EpidermisLabellingThread.cc \
	PluginEpidermisLabelling.cc \
	$(MOC_GEN_SOURCES)

moc_%.cc: %.hh
	$(MOC) $(AM_CPPFLAGS) $< -o $@

MOSTLYCLEANFILES = $(MOC_GEN_SOURCES) 
