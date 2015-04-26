lib_LTLIBRARIES = libBlitzFFTW.la
blitzfftwincludedir = $(pkgincludedir)/libBlitzFFTW

AM_CPPFLAGS = -I$(top_srcdir)/src
AM_LDFLAGS = -version-info 1:0:0

libBlitzFFTW_la_SOURCES = \
	BlitzFFTW.cc \
	BlitzFFTW-Impl.cc

blitzfftwinclude_HEADERS = \
	BlitzFFTW.hh \
	BlitzFFTW.icc \
	BlitzFFTW-Impl.hh \
	BlitzFFTWError.hh

