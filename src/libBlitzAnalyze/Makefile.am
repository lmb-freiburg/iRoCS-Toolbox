lib_LTLIBRARIES = libBlitzAnalyze.la
blitzanalyzeincludedir = $(pkgincludedir)/libBlitzAnalyze

AM_CPPFLAGS = -I$(top_srcdir)/src $(BLITZ_CFLAGS)
AM_LDFLAGS = -no-undefined -version-info 1:0:0

libBlitzAnalyze_la_LIBADD = $(BLITZ_LIBS)

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

