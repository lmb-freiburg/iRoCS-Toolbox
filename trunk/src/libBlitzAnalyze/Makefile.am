lib_LTLIBRARIES = libBlitzAnalyze.la
blitzanalyzeincludedir = $(pkgincludedir)/libBlitzAnalyze

AM_CPPFLAGS = -I$(top_srcdir)/src
AM_LDFLAGS = -version-info 1:0:0

blitzanalyzeinclude_HEADERS = \
	BlitzAnalyzeError.hh \
	BlitzAnalyzeFile.hh \
	BlitzAnalyzeTraits.hh \
	BlitzAnalyzeFile.icc \
	BlitzAnalyzeTraits.icc \
	BlitzAnalyzeType.hh

libBlitzAnalyze_la_SOURCES = \
	BlitzAnalyzeTraits.cc \
	BlitzAnalyzeFile.cc

