bin_PROGRAMS = assignLayersToSegmentation

AM_CPPFLAGS = -Wno-long-long -I$(top_srcdir)/src

if STATIC_TOOLS
AM_LDFLAGS = -static
endif

LDADD = $(top_builddir)/src/libIRoCS/libIRoCS.la \
	$(top_builddir)/src/libsvmtl/libsvmtl.la \
	$(top_builddir)/src/liblabelling_qt4/libLabelling_qt4.la \
	$(top_builddir)/src/libArrayToolbox/libArrayToolbox.la \
	$(top_builddir)/src/libsegmentation/libsegmentation.la \
	$(top_builddir)/src/lmbs2kit/liblmbs2kit.la \
	$(top_builddir)/src/libBlitzHdf5/libBlitzHdf5.la \
	$(top_builddir)/src/libBlitzAnalyze/libBlitzAnalyze.la \
	$(top_builddir)/src/libProgressReporter/libProgressReporter.la \
	$(top_builddir)/src/libcmdline/libcmdline.la

assignLayersToSegmentation_SOURCES = assignLayersToSegmentation.cc
