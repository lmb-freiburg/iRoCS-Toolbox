TESTS = testCopyFromToATBNucleus

check_PROGRAMS = $(TESTS)

AM_CPPFLAGS = -I$(top_srcdir)/src $(QT4_CFLAGS) $(HDF5_CFLAGS)
AM_CXXFLAGS = -Wno-long-long

LDADD = $(top_builddir)/src/liblabelling_qt4/libLabelling_qt4.la \
	$(top_builddir)/src/libsegmentation/libsegmentation.la \
	$(top_builddir)/src/libArrayToolbox/libArrayToolbox.la \
	$(top_builddir)/src/lmbs2kit/liblmbs2kit.la \
	$(top_builddir)/src/libBlitzHdf5/libBlitzHdf5.la \
	$(top_builddir)/src/libBlitzAnalyze/libBlitzAnalyze.la \
	$(top_builddir)/src/libProgressReporter/libProgressReporter.la \
	$(QT4_LIBS) $(HDF5_LIBS)

noinst_HEADERS = lmbunit.hh

testCopyFromToATBNucleus_SOURCES = testCopyFromToATBNucleus.cc
