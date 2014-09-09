lib_LTLIBRARIES = libBlitzHdf5.la
blitzhdf5includedir = $(pkgincludedir)/libBlitzHdf5

AM_CPPFLAGS = -I$(top_srcdir)/src
AM_CXXFLAGS = -Wno-long-long
AM_LDFLAGS = -version-info 1:0:0

libBlitzHdf5_la_SOURCES = \
	BlitzH5Traits.cc \
	BlitzHdf5Light.cc

blitzhdf5include_HEADERS = \
	BlitzH5Traits.hh \
	BlitzHdf5Light.hh BlitzHdf5Light.icc \
	FileIOWrapper.hh
