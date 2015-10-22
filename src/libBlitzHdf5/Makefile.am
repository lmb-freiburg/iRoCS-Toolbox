lib_LTLIBRARIES = libBlitzHdf5.la
blitzhdf5includedir = $(pkgincludedir)/libBlitzHdf5

AM_CPPFLAGS = -I$(top_srcdir)/src $(BLITZ_CFLAGS) $(HDF5_CFLAGS)
AM_CXXFLAGS = -Wno-long-long
AM_LDFLAGS = -no-undefined -version-info 3:0:2

libBlitzHdf5_la_LIBADD = \
	$(top_builddir)/src/libProgressReporter/libProgressReporter.la \
	$(BLITZ_LIBS) $(HDF5_LIBS)

libBlitzHdf5_la_SOURCES = \
	BlitzH5Traits.cc \
	BlitzHdf5Light.cc

blitzhdf5include_HEADERS = \
	BlitzH5Traits.hh \
	BlitzHdf5Light.hh BlitzHdf5Light.icc \
	FileIOWrapper.hh FileIOWrapper.icc
