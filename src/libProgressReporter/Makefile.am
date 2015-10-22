lib_LTLIBRARIES = libProgressReporter.la
progressreporterincludedir = $(pkgincludedir)/libProgressReporter

AM_CXXFLAGS = -Wno-long-long
AM_LDFLAGS = -no-undefined -version-info 1:0:0

# version bump to 2:0:0 pending (Incompatibel update due to library split)

libProgressReporter_la_SOURCES = \
	ProgressReporter.cc \
	ProgressReporterStream.cc

progressreporterinclude_HEADERS = \
	ProgressReporter.hh \
	ProgressReporterStream.hh
