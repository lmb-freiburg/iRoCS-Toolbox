lib_LTLIBRARIES = libBlitz2DGraphics.la
blitz2dgraphicsincludedir = $(pkgincludedir)/libBlitz2DGraphics

AM_CPPFLAGS = -I$(top_srcdir)/src
AM_CXXFLAGS = -Wno-long-long
AM_LDFLAGS = -version-info 1:0:0

# Version bump to 1:1:0 pending due to bugfix

blitz2dgraphicsinclude_HEADERS = \
	Blitz2DGraphics.hh \
        ImageAccessWrapper.hh \
        ImageTools.hh \
        ImageTools.icc \
        BlitzImageReader.hh \
        BlitzImageWriter.hh \
        ColorConvert.hh \
	BlitzRGBColor.hh \
        Blitz2Ddraw.hh \
        Blitz2Ddraw.icc \
        Blitz2Dgeometry.hh \
        Blitz2Dgeometry.icc

libBlitz2DGraphics_la_SOURCES = \
	ImageAccessWrapper.cc \
        BlitzImageReader.cc \
        BlitzImageWriter.cc \
	BlitzRGBColor.cc \
        ColorConvert.cc

