TESTS =  testLibBlitz2DGraphics

check_PROGRAMS = $(TESTS)

AM_CPPFLAGS = -I$(top_srcdir)/src $(HDF5_CFLAGS)
AM_CXXFLAGS = -Wno-long-long

LDADD = $(top_builddir)/src/libBlitz2DGraphics/libBlitz2DGraphics.la \
	$(top_builddir)/src/libBlitzHdf5/libBlitzHdf5.la \
	$(HDF5_LIBS)

testLibBlitz2DGraphics_SOURCES  = testLibBlitzGraphics.cc
