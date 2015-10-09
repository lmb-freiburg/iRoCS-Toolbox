lib_LTLIBRARIES = libProgressReporter_qt4.la
progressreporterincludedir = $(pkgincludedir)/libProgressReporter_qt4

MOC = @QT4_MOC@

AM_CPPFLAGS = -I$(top_srcdir)/src
AM_CXXFLAGS = -Wno-long-long $(QT4_CFLAGS)
AM_LDFLAGS = -version-info 0:0:0

libProgressReporter_qt4_la_LIBADD = $(QT4_LIBS)

MOC_GEN_SOURCES = \
	moc_ProgressReporterQt4.cc

libProgressReporter_qt4_la_SOURCES = \
	ProgressReporterQt4.cc \
	$(MOC_GEN_SOURCES)

progressreporterinclude_HEADERS = \
	ProgressReporterQt4.hh

moc_%.cc: %.hh
	$(MOC) $< -o $@

MOSTLYCLEANFILES = $(MOC_GEN_SOURCES) 
