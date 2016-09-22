TESTS = \
	testATBLinAlg \
	testArray \
	testLocalSumFilter

check_PROGRAMS = $(TESTS)

AM_CPPFLAGS = -I$(top_srcdir)/src $(GSL_CFLAGS) $(HDF5_CFLAGS) \
	-DTOP_BUILD_DIR="\"$(shell (cd \$(top_builddir); pwd))\""
AM_CXXFLAGS = -Wno-long-long

LDADD = $(top_builddir)/src/libArrayToolbox/libArrayToolbox.la \
	$(top_builddir)/src/libBlitzFFTW/libBlitzFFTW.la \
	$(top_builddir)/src/libBlitzHdf5/libBlitzHdf5.la \
	$(top_builddir)/src/libProgressReporter/libProgressReporter.la \
	$(top_builddir)/src/libBaseFunctions/libBaseFunctions.la \
	$(GSL_LIBS) $(HDF5_LIBS)

noinst_HEADERS = lmbunit.hh

testATBLinAlg_SOURCES = testATBLinAlg.cc
testArray_SOURCES = testArray.cc
testLocalSumFilter_SOURCES = testLocalSumFilter.cc

