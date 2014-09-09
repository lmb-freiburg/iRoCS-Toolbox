lblplugindir = $(pkgdatadir)/plugins
lblplugin_LTLIBRARIES = libPluginMitosisClassification.la
MOC = @QT4_MOC@

AM_CPPFLAGS = -I$(top_srcdir)/src
AM_CXXFLAGS = -Wno-long-long

libPluginMitosisClassification_la_LIBADD = \
	$(top_builddir)/src/libIRoCS/libIRoCS.la

MOC_GEN_SOURCES = \
	moc_MitosisClassificationParametersDialog.cc \
	moc_PluginMitosisClassification.cc

libPluginMitosisClassification_la_SOURCES = \
	MitosisClassificationParametersDialog.cc \
	PluginMitosisClassification.cc \
	$(MOC_GEN_SOURCES)

moc_%.cc: %.hh
	$(MOC) $(AM_CPPFLAGS) $< -o $@

MOSTLYCLEANFILES = $(MOC_GEN_SOURCES) 
