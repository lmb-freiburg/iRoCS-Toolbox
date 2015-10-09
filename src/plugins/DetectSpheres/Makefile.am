lblplugindir = $(pkgdatadir)/plugins
lblplugin_LTLIBRARIES = libPluginDetectSpheres.la
MOC = @QT4_MOC@

AM_CPPFLAGS = -I$(top_srcdir)/src $(BLITZ_CFLAGS) $(HDF5_CFLAGS) \
	$(FFTW_CFLAGS) $(GSL_CFLAGS) $(OPENCV_CFLAGS) $(QT4_CFLAGS)
AM_CXXFLAGS = -Wno-long-long
AM_LDFLAGS = -version-info 1:0:0

libPluginDetectSpheres_la_LIBADD = $(top_builddir)/src/libIRoCS/libIRoCS.la

LIBADD = $(top_builddir)/src/libIRoCS/libIRoCS.la

MOC_GEN_SOURCES = \
	moc_DetectSpheresParametersDialog.cc \
	moc_DetectSpheresThread.cc \
	moc_PluginDetectSpheres.cc

libPluginDetectSpheres_la_SOURCES = \
	DetectSpheresParametersDialog.cc \
	DetectSpheresThread.cc \
	PluginDetectSpheres.cc \
	$(MOC_GEN_SOURCES)

moc_%.cc: %.hh
	$(MOC) $(AM_CPPFLAGS) $< -o $@

MOSTLYCLEANFILES = $(MOC_GEN_SOURCES) 
