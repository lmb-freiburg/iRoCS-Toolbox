TESTS = testProgressReporterStream testProgressReporterQt4

check_PROGRAMS = $(TESTS)

AM_CPPFLAGS = -I$(top_srcdir)/src $(QT4_CFLAGS)
AM_CXXFLAGS = -Wno-long-long

testProgressReporterStream_LDADD = \
	$(top_builddir)/src/libProgressReporter/libProgressReporter.la
testProgressReporterQt4_LDADD = \
	$(top_builddir)/src/libProgressReporter_qt4/libProgressReporter_qt4.la \
	$(QT4_LIBS)

noinst_HEADERS = lmbunit.hh

testProgressReporterStream_SOURCES = testProgressReporterStream.cc
testProgressReporterQt4_SOURCES = testProgressReporterQt4.cc
