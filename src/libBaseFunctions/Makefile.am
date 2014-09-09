lib_LTLIBRARIES = libBaseFunctions.la
basefunctionsincludedir = $(pkgincludedir)/libBaseFunctions

AM_CPPFLAGS = -I$(top_srcdir)/src
AM_LDFLAGS = -version-info @lt_major@:@lt_revision@:@lt_age@

basefunctionsinclude_HEADERS = \
    BaseFile.hh \
    BaseEnvironment.hh

libBaseFunctions_la_SOURCES = \
    BaseFile.cc \
    BaseEnvironment.cc
