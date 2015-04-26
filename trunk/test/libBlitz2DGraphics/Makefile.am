TESTS =  testLibBlitzGraphics

check_PROGRAMS = $(TESTS)

AM_CPPFLAGS = -I$(top_srcdir)/src
AM_CXXFLAGS = -Wno-long-long

LDADD = $(top_builddir)/src/libBlitzHdf5/libBlitzHdf5.la

testLibBlitzGraphics_SOURCES  = testLibBlitzGraphics.cc
