lblplugindir = $(pkgdatadir)/plugins
lblplugin_LTLIBRARIES = libPluginAttachIRoCSSCTToCellSegmentation.la
MOC = @QT4_MOC@

AM_CPPFLAGS = -I$(top_srcdir)/src
AM_CXXFLAGS = -Wno-long-long
AM_LDFLAGS = -version-info 1:0:0

libPluginAttachIRoCSSCTToCellSegmentation_la_LIBADD = \
	$(top_builddir)/src/libIRoCS/libIRoCS.la

MOC_GEN_SOURCES = \
	moc_AttachIRoCSSCTToCellSegmentationParametersDialog.cc \
	moc_AttachIRoCSSCTToCellSegmentationThread.cc \
	moc_PluginAttachIRoCSSCTToCellSegmentation.cc

libPluginAttachIRoCSSCTToCellSegmentation_la_SOURCES = \
	AttachIRoCSSCTToCellSegmentationParametersDialog.cc \
	AttachIRoCSSCTToCellSegmentationThread.cc \
	PluginAttachIRoCSSCTToCellSegmentation.cc \
	$(MOC_GEN_SOURCES)

moc_%.cc: %.hh
	$(MOC) $(AM_CPPFLAGS) $< -o $@

MOSTLYCLEANFILES = $(MOC_GEN_SOURCES) 
