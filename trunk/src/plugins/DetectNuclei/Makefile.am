lblplugindir = $(pkgdatadir)/plugins
lblplugin_LTLIBRARIES = libPluginDetectNuclei.la
MOC = @QT4_MOC@

AM_CPPFLAGS = -I$(top_srcdir)/src
AM_CXXFLAGS = -Wno-long-long
AM_LDFLAGS = -version-info 1:0:0

libPluginDetectNuclei_la_LIBADD = \
	$(top_builddir)/src/libIRoCS/libIRoCS.la

MOC_GEN_SOURCES = \
	moc_DetectNucleiParametersDialog.cc \
	moc_DetectNucleiThread.cc \
	moc_PluginDetectNuclei.cc

libPluginDetectNuclei_la_SOURCES = \
	DetectNucleiParametersDialog.cc \
	DetectNucleiThread.cc \
	PluginDetectNuclei.cc \
	$(MOC_GEN_SOURCES)

moc_%.cc: %.hh
	$(MOC) $(AM_CPPFLAGS) $< -o $@

MOSTLYCLEANFILES = $(MOC_GEN_SOURCES) 
