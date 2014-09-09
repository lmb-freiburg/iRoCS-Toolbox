lib_LTLIBRARIES = libProgressReporter.la
progressreporterincludedir = $(pkgincludedir)/libProgressReporter

MOC = @QT4_MOC@

AM_CXXFLAGS = -Wno-long-long
AM_LDFLAGS = -version-info 1:0:0

MOC_GEN_SOURCES = \
	moc_ProgressReporterQt4.cc

libProgressReporter_la_SOURCES = \
	ProgressReporter.cc \
	ProgressReporterStream.cc \
	ProgressReporterQt4.cc \
	$(MOC_GEN_SOURCES)

progressreporterinclude_HEADERS = \
	ProgressReporter.hh \
	ProgressReporterStream.hh \
	ProgressReporterQt4.hh

moc_%.cc: %.hh
	$(MOC) $< -o $@

MOSTLYCLEANFILES = $(MOC_GEN_SOURCES) 
