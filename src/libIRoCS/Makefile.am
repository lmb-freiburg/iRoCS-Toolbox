lib_LTLIBRARIES = libIRoCS.la
irocsincludedir = $(pkgincludedir)/libIRoCS

MOC = @QT4_MOC@

AM_CPPFLAGS = -I$(top_srcdir)/src
AM_CXXFLAGS = -Wno-long-long
AM_LDFLAGS = -version-info 2:0:0

irocsinclude_HEADERS = \
	iRoCSFeatures.hh iRoCSFeatures.icc \
	DetectNucleiWorker.hh \
	EpidermisLabellingWorker.hh \
	AttachIRoCSWorker.hh \
	LayerAssignmentWorker.hh \
	MitosisClassificationParameters.hh \
	MitosisClassificationWorker.hh \
	RootSegmentationWorker.hh \
	AttachIRoCSToCellSegmentationParameters.hh \
	AttachIRoCSToCellSegmentationWorker.hh \
	AttachIRoCSSCTToCellSegmentationWorker.hh \
	ComputeCellFeaturesWorker.hh \
	AssignLayersToCellSegmentationWorker.hh \
	TrainfileParameters.hh \
	TrainingParameters.hh \
	TrainDetectorWorker.hh \
	TrainEpidermisLabellingWorker.hh \
	TrainLayerAssignmentWorker.hh \
	DetectSpheresWorker.hh

MOC_GEN_SOURCES = \
	moc_MitosisClassificationWorker.cc \
	moc_AttachIRoCSToCellSegmentationWorker.cc \
        moc_TrainDetectorWorker.cc \
        moc_TrainEpidermisLabellingWorker.cc \
        moc_TrainLayerAssignmentWorker.cc

libIRoCS_la_SOURCES = \
	iRoCSFeatures.cc \
	DetectNucleiWorker.cc \
	EpidermisLabellingWorker.cc \
	AttachIRoCSWorker.cc \
	LayerAssignmentWorker.cc \
	MitosisClassificationParameters.cc \
	MitosisClassificationWorker.cc \
	RootSegmentationWorker.cc \
	AttachIRoCSToCellSegmentationParameters.cc \
	AttachIRoCSToCellSegmentationWorker.cc \
	AttachIRoCSSCTToCellSegmentationWorker.cc \
	ComputeCellFeaturesWorker.cc \
	AssignLayersToCellSegmentationWorker.cc \
	TrainfileParameters.cc \
	TrainingParameters.cc \
	TrainDetectorWorker.cc \
	TrainEpidermisLabellingWorker.cc \
	TrainLayerAssignmentWorker.cc \
	DetectSpheresWorker.cc \
	$(MOC_GEN_SOURCES)

moc_%.cc: %.hh
	$(MOC) $(INCLUDES) $< -o $@

MOSTLYCLEANFILES = $(MOC_GEN_SOURCES) 
