bin_PROGRAMS = \
	h5cp h5rm existsDataset existsAttribute writeAttribute \
	setElementSizeUm setTransformationMatrix cropDataset \
	cropAnnotationChannel scaleDataset

AM_CPPFLAGS = -I$(top_srcdir)/src $(BLITZ_CFLAGS) $(HDF5_CFLAGS) \
	$(HDF5_CPP_CFLAGS) $(FFTW_CFLAGS) $(GSL_CFLAGS) $(OPENCV_CFLAGS)
AM_CXXFLAGS = -Wno-long-long -std=c++11

LDADD = \
	$(top_builddir)/src/libArrayToolbox/libArrayToolbox.la \
	$(top_builddir)/src/libBlitzHdf5/libBlitzHdf5.la \
	$(top_builddir)/src/libBlitzAnalyze/libBlitzAnalyze.la \
	$(top_builddir)/src/libProgressReporter/libProgressReporter.la \
	$(top_builddir)/src/libsvmtl/libsvmtl.la \
	$(top_builddir)/src/libcmdline/libcmdline.la \
	$(top_builddir)/src/lmbs2kit/liblmbs2kit.la

if STATIC_TOOLS
AM_LDFLAGS = -all-static
LDADD += $(HDF5_CPP_STATIC_LIBS) $(HDF5_STATIC_LIBS) $(GSL_STATIC_LIBS) \
	$(ZLIB_STATIC_LIBS) $(SZIP_STATIC_LIBS)
else
LDADD += $(HDF5_CPP_LIBS) $(HDF5_LIBS) $(GSL_LIBS)
endif

h5cp_SOURCES = h5cp.cc
h5rm_SOURCES = h5rm.cc
existsDataset_SOURCES = existsDataset.cc
existsAttribute_SOURCES = existsAttribute.cc
writeAttribute_SOURCES = writeAttribute.cc
setElementSizeUm_SOURCES = setElementSizeUm.cc
setTransformationMatrix_SOURCES = setTransformationMatrix.cc
cropDataset_SOURCES = cropDataset.cc
cropAnnotationChannel_SOURCES = cropAnnotationChannel.cc
scaleDataset_SOURCES = scaleDataset.cc
