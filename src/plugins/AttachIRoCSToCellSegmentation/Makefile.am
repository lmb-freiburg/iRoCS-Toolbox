lblplugindir = $(pkgdatadir)/plugins
lblplugin_LTLIBRARIES = libPluginAttachIRoCSToCellSegmentation.la
MOC = @QT4_MOC@

AM_CPPFLAGS = -I$(top_srcdir)/src
AM_CXXFLAGS = -Wno-long-long
AM_LDFLAGS = -version-info 1:0:0

libPluginAttachIRoCSToCellSegmentation_la_LIBADD = \
	$(top_builddir)/src/libIRoCS/libIRoCS.la

MOC_GEN_SOURCES = \
	moc_AttachIRoCSToCellSegmentationParametersDialog.cc \
	moc_PluginAttachIRoCSToCellSegmentation.cc

libPluginAttachIRoCSToCellSegmentation_la_SOURCES = \
	AttachIRoCSToCellSegmentationParametersDialog.cc \
	PluginAttachIRoCSToCellSegmentation.cc \
	$(MOC_GEN_SOURCES)

moc_%.cc: %.hh
	$(MOC) $(AM_CPPFLAGS) $< -o $@

MOSTLYCLEANFILES = $(MOC_GEN_SOURCES) 
