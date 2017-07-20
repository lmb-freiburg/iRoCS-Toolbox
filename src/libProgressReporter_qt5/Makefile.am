lib_LTLIBRARIES = libProgressReporter_qt5.la
progressreporterincludedir = $(pkgincludedir)/libProgressReporter_qt5

MOC = @QT5_MOC@

AM_CPPFLAGS = -I$(top_srcdir)/src $(QT5_CFLAGS)
AM_CXXFLAGS = -Wno-long-long
AM_LDFLAGS = -no-undefined -version-info 1:0:0

libProgressReporter_qt5_la_LIBADD = \
	$(top_builddir)/src/libProgressReporter/libProgressReporter.la \
	$(QT5_LIBS)

MOC_GEN_SOURCES = \
	moc_ProgressReporterQt5.cc

libProgressReporter_qt5_la_SOURCES = \
	ProgressReporterQt5.cc

nodist_libProgressReporter_qt5_la_SOURCES = \
	$(MOC_GEN_SOURCES)

progressreporterinclude_HEADERS = \
	ProgressReporterQt5.hh

moc_%.cc: %.hh
	$(MOC) $< -o $@

MOSTLYCLEANFILES = $(MOC_GEN_SOURCES)
