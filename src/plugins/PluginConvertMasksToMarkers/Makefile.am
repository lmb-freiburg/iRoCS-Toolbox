lblplugindir = $(pkgdatadir)/plugins
lblplugin_LTLIBRARIES = libPluginConvertMasksToMarkers.la
MOC = @QT4_MOC@

AM_CPPFLAGS = -I$(top_srcdir)/src
AM_CXXFLAGS = -Wno-long-long
AM_LDFLAGS = -version-info 1:0:0

MOC_GEN_SOURCES = \
	moc_ConvertMasksToMarkersParameters.cc \
	moc_ConvertMasksToMarkersWorker.cc \
	moc_PluginConvertMasksToMarkers.cc

libPluginConvertMasksToMarkers_la_SOURCES = \
	ConvertMasksToMarkersParameters.cc \
	ConvertMasksToMarkersWorker.cc \
	PluginConvertMasksToMarkers.cc \
	$(MOC_GEN_SOURCES)

moc_%.cc: %.hh
	$(MOC) $(AM_CPPFLAGS) $< -o $@

MOSTLYCLEANFILES = $(MOC_GEN_SOURCES)
