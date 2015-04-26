TESTS = testProgressReporterStream testProgressReporterQt4

check_PROGRAMS = $(TESTS)

AM_CPPFLAGS = -I$(top_srcdir)/src
AM_CXXFLAGS = -Wno-long-long

LDADD = $(top_builddir)/src/libProgressReporter/libProgressReporter.la

testProgressReporterStream_SOURCES = testProgressReporterStream.cc
testProgressReporterQt4_SOURCES = testProgressReporterQt4.cc
