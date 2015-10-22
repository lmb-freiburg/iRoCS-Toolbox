TESTS = testBlitzAnalyzeFile

check_PROGRAMS = $(TESTS)

AM_CPPFLAGS = -I$(top_srcdir)/src -DTOP_BUILD_DIR="\"$(shell (cd \$(top_builddir); pwd))\""
AM_CXXFLAGS = -Wno-long-long

LDADD = $(top_builddir)/src/libBlitzAnalyze/libBlitzAnalyze.la

noinst_HEADERS = lmbunit.hh

testBlitzAnalyzeFile_SOURCES = testBlitzAnalyzeFile.cc
