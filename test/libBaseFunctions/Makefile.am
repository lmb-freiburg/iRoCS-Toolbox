TESTS = testBaseEnvironment testBaseFile

check_PROGRAMS = $(TESTS)

AM_CPPFLAGS = -I$(top_srcdir)/src -DTOP_BUILD_DIR="\"$(shell (cd \$(top_builddir); pwd))\""
AM_CXXFLAGS = -Wno-long-long

LDADD = $(top_builddir)/src/libBaseFunctions/libBaseFunctions.la

noinst_HEADERS = lmbunit.hh

testBaseEnvironment_SOURCES = testBaseEnvironment.cc
testBaseFile_SOURCES = testBaseFile.cc
