bin_PROGRAMS = assignLayers

AM_CPPFLAGS = -I$(top_srcdir)/src $(BLITZ_CFLAGS) $(HDF5_CFLAGS) \
	$(HDF5_CPP_CFLAGS) $(FFTW_CFLAGS) $(GSL_CFLAGS) $(OPENCV_CFLAGS)
AM_CXXFLAGS = -Wno-long-long

assignLayers_LDADD = \
	$(top_builddir)/src/libIRoCS/libIRoCS.la \
	$(top_builddir)/src/libsegmentation/libsegmentation.la \
	$(top_builddir)/src/libArrayToolbox/libArrayToolbox.la \
	$(top_builddir)/src/libBlitzHdf5/libBlitzHdf5.la \
	$(top_builddir)/src/libBlitzAnalyze/libBlitzAnalyze.la \
	$(top_builddir)/src/libProgressReporter/libProgressReporter.la \
	$(top_builddir)/src/libsvmtl/libsvmtl.la \
	$(top_builddir)/src/libcmdline/libcmdline.la \
	$(top_builddir)/src/lmbs2kit/liblmbs2kit.la

if STATIC_TOOLS
assignLayers_LDFLAGS = -all-static
assignLayers_LDADD += $(HDF5_CPP_STATIC_LIBS) $(HDF5_STATIC_LIBS) \
	$(ZLIB_STATIC_LIBS) $(SZIP_STATIC_LIBS)
else
assignLayers_LDADD += $(HDF5_CPP_LIBS) $(HDF5_LIBS)
endif

assignLayers_SOURCES = assignLayers.cc
