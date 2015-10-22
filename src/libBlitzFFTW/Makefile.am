lib_LTLIBRARIES = libBlitzFFTW.la
blitzfftwincludedir = $(pkgincludedir)/libBlitzFFTW

AM_CPPFLAGS = -I$(top_srcdir)/src $(BLITZ_CFLAGS) $(FFTW_CFLAGS)
AM_LDFLAGS = -no-undefined -version-info 1:0:0

libBlitzFFTW_la_LIBADD = \
	$(top_builddir)/src/libBaseFunctions/libBaseFunctions.la \
	$(BLITZ_LIBS) $(FFTW_LIBS)

libBlitzFFTW_la_SOURCES = \
	BlitzFFTW.cc \
	BlitzFFTW-Impl.cc

blitzfftwinclude_HEADERS = \
	BlitzFFTW.hh \
	BlitzFFTW.icc \
	BlitzFFTW-Impl.hh \
	BlitzFFTWError.hh

