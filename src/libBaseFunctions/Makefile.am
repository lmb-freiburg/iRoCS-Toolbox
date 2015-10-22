lib_LTLIBRARIES = libBaseFunctions.la
basefunctionsincludedir = $(pkgincludedir)/libBaseFunctions

AM_CPPFLAGS = -I$(top_srcdir)/src
AM_LDFLAGS = -no-undefined -version-info 1:0:0

basefunctionsinclude_HEADERS = \
    BaseFile.hh \
    BaseEnvironment.hh

libBaseFunctions_la_SOURCES = \
    BaseFile.cc \
    BaseEnvironment.cc
