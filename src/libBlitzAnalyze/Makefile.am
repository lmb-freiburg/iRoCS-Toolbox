lib_LTLIBRARIES = libBlitzAnalyze.la
blitzanalyzeincludedir = $(pkgincludedir)/libBlitzAnalyze

AM_CPPFLAGS = -I$(top_srcdir)/src
AM_LDFLAGS = -version-info @lt_major@:@lt_revision@:@lt_age@

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

