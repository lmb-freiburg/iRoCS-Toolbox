TESTS = testSimplifyGroupDescriptor \
	testH5FileConstructor \
	testH5FileReadDataset \
	testH5FileWriteDataset \
	testDatasetShape \
	testDatasetType \
	testDeleteDataset \
	testH5FileReadAttribute \
	testH5FileWriteAttribute \
	testGetObjects

check_PROGRAMS = $(TESTS)

AM_CPPFLAGS = -I$(top_srcdir)/src $(HDF5_CFLAGS)
AM_CXXFLAGS = -Wno-long-long

LDADD = $(top_builddir)/src/libBlitzHdf5/libBlitzHdf5.la \
	$(top_builddir)/src/libProgressReporter/libProgressReporter.la \
	$(HDF5_LIBS)

noinst_HEADERS = lmbunit.hh

testSimplifyGroupDescriptor_SOURCES = testSimplifyGroupDescriptor.cc
testH5FileConstructor_SOURCES = testH5FileConstructor.cc
testH5FileReadDataset_SOURCES = testH5FileReadDataset.cc
testH5FileWriteDataset_SOURCES = testH5FileWriteDataset.cc
testDatasetShape_SOURCES = testDatasetShape.cc
testDatasetType_SOURCES = testDatasetType.cc
testDeleteDataset_SOURCES = testDeleteDataset.cc
testH5FileReadAttribute_SOURCES = testH5FileReadAttribute.cc
testH5FileWriteAttribute_SOURCES = testH5FileWriteAttribute.cc
testGetObjects_SOURCES = testGetObjects.cc
