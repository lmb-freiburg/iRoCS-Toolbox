TESTS = testBlitzFFTW \
	testBlitzFFTWFloat

check_PROGRAMS = $(TESTS)

AM_CPPFLAGS = -I$(top_srcdir)/src
AM_CXXFLAGS = -Wno-long-long

LDADD = $(top_builddir)/src/libBlitzFFTW/libBlitzFFTW.la \
	$(top_builddir)/src/libBaseFunctions/libBaseFunctions.la

noinst_HEADERS = lmbunit.hh

testBlitzFFTW_SOURCES = testBlitzFFTW.cc
testBlitzFFTWFloat_SOURCES = testBlitzFFTWFloat.cc
