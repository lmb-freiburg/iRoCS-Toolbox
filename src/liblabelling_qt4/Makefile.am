lib_LTLIBRARIES = libLabelling_qt4.la
lbldatadir = $(pkgdatadir)
labelling_qt4includedir = $(pkgincludedir)/libLabelling_qt4

MOC = @QT4_MOC@
RCC = @QT4_RCC@

AM_CPPFLAGS = -I$(top_srcdir)/src \
	$(GSL_CFLAGS) $(OPENCV_CFLAGS) $(BLITZ_CFLAGS) $(FFTW_CFLAGS) \
	$(HDF5_CFLAGS) $(QT4_CFLAGS)
AM_CXXFLAGS = -Wno-long-long
AM_LDFLAGS = -no-undefined -version-info 3:0:0 -rdynamic

libLabelling_qt4_la_LIBADD = \
	$(top_builddir)/src/libsegmentation/libsegmentation.la \
	$(top_builddir)/src/libArrayToolbox/libArrayToolbox.la \
	$(top_builddir)/src/libBlitzFFTW/libBlitzFFTW.la \
	$(top_builddir)/src/libBlitzHdf5/libBlitzHdf5.la \
	$(top_builddir)/src/libBaseFunctions/libBaseFunctions.la \
	$(top_builddir)/src/lmbs2kit/liblmbs2kit.la \
	$(top_builddir)/src/libBlitzAnalyze/libBlitzAnalyze.la \
	$(top_builddir)/src/libProgressReporter_qt4/libProgressReporter_qt4.la \
	$(top_builddir)/src/libProgressReporter/libProgressReporter.la \
	$(BLITZ_LIBS) $(HDF5_LIBS) $(FFTW_LIBS) $(GSL_LIBS) $(OPENCV_LIBS) \
	$(QT4_LIBS) $(GL_LIBS) $(GLU_LIBS)

lbldata_DATA = liblabelling_qt4.rcc

EXTRA_DIST = \
	icons/labellingIcon.svg \
	icons/oxygen/32x32/actions/document-properties.png \
	icons/oxygen/32x32/actions/edit-delete.png \
	icons/oxygen/32x32/actions/process-stop.png \
	icons/oxygen/32x32/actions/zoom-in.png \
	icons/oxygen/32x32/actions/zoom-original.png \
	icons/oxygen/32x32/actions/zoom-out.png

labelling_qt4include_HEADERS = \
    StreamBufTextEditQt4.hh \
    AbstractControlElement.hh \
    BoolControlElement.hh \
    IntControlElement.hh \
    Int3ControlElement.hh \
    IntVectorControlElement.hh \
    DoubleControlElement.hh \
    Double3ControlElement.hh \
    DoubleVectorControlElement.hh \
    Double4x4ControlElement.hh \
    DoubleRangeControlElement.hh \
    StringControlElement.hh \
    StringSelectionControlElement.hh \
    IntDoubleMapControlElement.hh \
    ChannelSelectionControlElement.hh \
    FileNameSelectionControlElement.hh \
    HDF5SelectionControlElement.hh \
    ColorControlElement.hh \
    ColorMap.hh \
    IColorMapEditor.hh \
    ColorMapEditorWidget.hh \
    UserInteractionEvent.hh \
    OrthoViewUserInteractionEvent.hh \
    Marker.hh \
    MarkerPresetWidget.hh \
    MarkerControlWidget.hh \
    PointMarker.hh \
    SphereMarker.hh \
    SHSurfaceMarker.hh \
    NucleusMarker.hh \
    CylinderMarker.hh \
    SurfaceMarker.hh \
    CellMarker.hh \
    ChannelSpecs.hh \
    DataChannelSpecs.hh \
    RGBChannelSpecs.hh \
    VisualizationChannelSpecs.hh \
    AnnotationChannelSpecs.hh \
    SplineCurveChannelSpecs.hh \
    IRoCSChannelSpecs.hh \
    IRoCSSCTChannelSpecs.hh \
    ViewWidget.hh \
    MarkerRenderer.hh \
    ChannelSpecsRenderer.hh \
    OrthoViewPlane.hh \
    OrthoViewWidget.hh \
    ChannelSpecsOrthoViewRenderer.hh \
    MarkerOrthoViewRenderer.hh \
    PointMarkerOrthoViewRenderer.hh \
    SphereMarkerOrthoViewRenderer.hh \
    SHSurfaceMarkerOrthoViewRenderer.hh \
    NucleusMarkerOrthoViewRenderer.hh \
    CylinderMarkerOrthoViewRenderer.hh \
    SurfaceMarkerOrthoViewRenderer.hh \
    CellMarkerOrthoViewRenderer.hh \
    DataChannelSpecsOrthoViewRenderer.hh \
    RGBChannelSpecsOrthoViewRenderer.hh \
    VisualizationChannelSpecsOrthoViewRenderer.hh \
    AnnotationChannelSpecsOrthoViewRenderer.hh \
    SplineCurveChannelSpecsOrthoViewRenderer.hh \
    IRoCSChannelSpecsOrthoViewRenderer.hh \
    IRoCSSCTChannelSpecsOrthoViewRenderer.hh \
    OpenGlRenderingWidget.hh \
    OpenGlRenderingViewWidget.hh \
    OpenGlRenderingSettingsWidget.hh \
    ChannelSpecsOpenGlRenderingViewRenderer.hh \
    DataChannelSpecsOpenGlRenderingViewRenderer.hh \
    RGBChannelSpecsOpenGlRenderingViewRenderer.hh \
    VisualizationChannelSpecsOpenGlRenderingViewRenderer.hh \
    AnnotationChannelSpecsOpenGlRenderingViewRenderer.hh \
    SplineCurveChannelSpecsOpenGlRenderingViewRenderer.hh \
    IRoCSChannelSpecsOpenGlRenderingViewRenderer.hh \
    IRoCSSCTChannelSpecsOpenGlRenderingViewRenderer.hh \
    MarkerOpenGlRenderingViewRenderer.hh \
    PointMarkerOpenGlRenderingViewRenderer.hh \
    SphereMarkerOpenGlRenderingViewRenderer.hh \
    NucleusMarkerOpenGlRenderingViewRenderer.hh \
    SHSurfaceMarkerOpenGlRenderingViewRenderer.hh \
    SurfaceMarkerOpenGlRenderingViewRenderer.hh \
    CellMarkerOpenGlRenderingViewRenderer.hh \
    CylinderMarkerOpenGlRenderingViewRenderer.hh \
    MultiChannelModel.hh \
    HDF5SelectionDialog.hh \
    SVGDataIO.hh \
    AnalyzeAndSTLDataIO.hh \
    HDF5DataIO.hh \
    CSVDataIO.hh \
    LabellingMainWidget.hh \
    OrthoViewOverlay.hh \
    ExceptionAwareApplication.hh \
    PluginInterface.hh

MOC_GEN_SOURCES = \
	moc_StreamBufTextEditQt4.cc \
	moc_AbstractControlElement.cc \
	moc_BoolControlElement.cc \
	moc_IntControlElement.cc \
	moc_Int3ControlElement.cc \
	moc_IntVectorControlElement.cc \
	moc_DoubleControlElement.cc \
	moc_Double3ControlElement.cc \
	moc_DoubleVectorControlElement.cc \
        moc_Double4x4ControlElement.cc \
	moc_DoubleRangeControlElement.cc \
	moc_StringControlElement.cc \
	moc_StringSelectionControlElement.cc \
	moc_IntDoubleMapControlElement.cc \
	moc_ChannelSelectionControlElement.cc \
	moc_FileNameSelectionControlElement.cc \
	moc_HDF5SelectionControlElement.cc \
	moc_ColorControlElement.cc \
	moc_Marker.cc \
	moc_MarkerPresetWidget.cc \
	moc_MarkerControlWidget.cc \
	moc_PointMarker.cc \
	moc_SphereMarker.cc \
	moc_SHSurfaceMarker.cc \
	moc_NucleusMarker.cc \
        moc_CylinderMarker.cc \
	moc_SurfaceMarker.cc \
	moc_CellMarker.cc \
	moc_ChannelSpecs.cc \
	moc_DataChannelSpecs.cc \
	moc_RGBChannelSpecs.cc \
	moc_VisualizationChannelSpecs.cc \
	moc_AnnotationChannelSpecs.cc \
	moc_SplineCurveChannelSpecs.cc \
	moc_IRoCSChannelSpecs.cc \
	moc_IRoCSSCTChannelSpecs.cc \
	moc_ViewWidget.cc \
	moc_MarkerRenderer.cc \
	moc_ChannelSpecsRenderer.cc \
	moc_OrthoViewPlane.cc \
	moc_OrthoViewWidget.cc \
	moc_ChannelSpecsOrthoViewRenderer.cc \
	moc_MarkerOrthoViewRenderer.cc \
	moc_PointMarkerOrthoViewRenderer.cc \
	moc_SphereMarkerOrthoViewRenderer.cc \
	moc_SHSurfaceMarkerOrthoViewRenderer.cc \
	moc_NucleusMarkerOrthoViewRenderer.cc \
        moc_CylinderMarkerOrthoViewRenderer.cc \
	moc_SurfaceMarkerOrthoViewRenderer.cc \
	moc_CellMarkerOrthoViewRenderer.cc \
	moc_DataChannelSpecsOrthoViewRenderer.cc \
	moc_RGBChannelSpecsOrthoViewRenderer.cc \
	moc_VisualizationChannelSpecsOrthoViewRenderer.cc \
	moc_AnnotationChannelSpecsOrthoViewRenderer.cc \
	moc_SplineCurveChannelSpecsOrthoViewRenderer.cc \
	moc_IRoCSChannelSpecsOrthoViewRenderer.cc \
	moc_IRoCSSCTChannelSpecsOrthoViewRenderer.cc \
	moc_OpenGlRenderingWidget.cc \
	moc_OpenGlRenderingViewWidget.cc \
	moc_OpenGlRenderingSettingsWidget.cc \
	moc_ChannelSpecsOpenGlRenderingViewRenderer.cc \
	moc_DataChannelSpecsOpenGlRenderingViewRenderer.cc \
	moc_RGBChannelSpecsOpenGlRenderingViewRenderer.cc \
	moc_VisualizationChannelSpecsOpenGlRenderingViewRenderer.cc \
	moc_AnnotationChannelSpecsOpenGlRenderingViewRenderer.cc \
	moc_SplineCurveChannelSpecsOpenGlRenderingViewRenderer.cc \
	moc_IRoCSChannelSpecsOpenGlRenderingViewRenderer.cc \
	moc_IRoCSSCTChannelSpecsOpenGlRenderingViewRenderer.cc \
	moc_MarkerOpenGlRenderingViewRenderer.cc \
	moc_PointMarkerOpenGlRenderingViewRenderer.cc \
	moc_SphereMarkerOpenGlRenderingViewRenderer.cc \
	moc_NucleusMarkerOpenGlRenderingViewRenderer.cc \
	moc_SHSurfaceMarkerOpenGlRenderingViewRenderer.cc \
	moc_SurfaceMarkerOpenGlRenderingViewRenderer.cc \
	moc_CellMarkerOpenGlRenderingViewRenderer.cc \
	moc_CylinderMarkerOpenGlRenderingViewRenderer.cc \
	moc_MultiChannelModel.cc \
	moc_HDF5SelectionDialog.cc \
	moc_SVGDataIO.cc \
	moc_HDF5DataIO.cc \
	moc_ColorMapEditorWidget.cc \
	moc_ExceptionAwareApplication.cc \
	moc_LabellingMainWidget.cc

libLabelling_qt4_la_SOURCES = \
	StreamBufTextEditQt4.cc \
	AbstractControlElement.cc \
	BoolControlElement.cc \
	IntControlElement.cc \
	Int3ControlElement.cc \
	IntVectorControlElement.cc \
	DoubleControlElement.cc \
	Double3ControlElement.cc \
	DoubleVectorControlElement.cc \
	Double4x4ControlElement.cc \
	DoubleRangeControlElement.cc \
	StringControlElement.cc \
	StringSelectionControlElement.cc \
	IntDoubleMapControlElement.cc \
	ChannelSelectionControlElement.cc \
	FileNameSelectionControlElement.cc \
	HDF5SelectionControlElement.cc \
	ColorControlElement.cc \
	Marker.cc \
	MarkerPresetWidget.cc \
	MarkerControlWidget.cc \
	PointMarker.cc \
	SphereMarker.cc \
	SHSurfaceMarker.cc \
	NucleusMarker.cc \
        CylinderMarker.cc \
	SurfaceMarker.cc \
	CellMarker.cc \
	UserInteractionEvent.cc \
	OrthoViewUserInteractionEvent.cc \
	ChannelSpecs.cc \
	DataChannelSpecs.cc \
	RGBChannelSpecs.cc \
	VisualizationChannelSpecs.cc \
	AnnotationChannelSpecs.cc \
	SplineCurveChannelSpecs.cc \
	IRoCSChannelSpecs.cc \
	IRoCSSCTChannelSpecs.cc \
	ViewWidget.cc \
	MarkerRenderer.cc \
	ChannelSpecsRenderer.cc \
	OrthoViewPlane.cc \
	OrthoViewWidget.cc \
	ChannelSpecsOrthoViewRenderer.cc \
	MarkerOrthoViewRenderer.cc \
	PointMarkerOrthoViewRenderer.cc \
	SphereMarkerOrthoViewRenderer.cc \
	SHSurfaceMarkerOrthoViewRenderer.cc \
	NucleusMarkerOrthoViewRenderer.cc \
        CylinderMarkerOrthoViewRenderer.cc \
	SurfaceMarkerOrthoViewRenderer.cc \
	CellMarkerOrthoViewRenderer.cc \
	DataChannelSpecsOrthoViewRenderer.cc \
	RGBChannelSpecsOrthoViewRenderer.cc \
	VisualizationChannelSpecsOrthoViewRenderer.cc \
	AnnotationChannelSpecsOrthoViewRenderer.cc \
	SplineCurveChannelSpecsOrthoViewRenderer.cc \
	IRoCSChannelSpecsOrthoViewRenderer.cc \
	IRoCSSCTChannelSpecsOrthoViewRenderer.cc \
	OpenGlRenderingWidget.cc \
	OpenGlRenderingViewWidget.cc \
	OpenGlRenderingSettingsWidget.cc \
	ChannelSpecsOpenGlRenderingViewRenderer.cc \
	DataChannelSpecsOpenGlRenderingViewRenderer.cc \
	RGBChannelSpecsOpenGlRenderingViewRenderer.cc \
	VisualizationChannelSpecsOpenGlRenderingViewRenderer.cc \
	AnnotationChannelSpecsOpenGlRenderingViewRenderer.cc \
	SplineCurveChannelSpecsOpenGlRenderingViewRenderer.cc \
	IRoCSChannelSpecsOpenGlRenderingViewRenderer.cc \
	IRoCSSCTChannelSpecsOpenGlRenderingViewRenderer.cc \
	MarkerOpenGlRenderingViewRenderer.cc \
	PointMarkerOpenGlRenderingViewRenderer.cc \
	SphereMarkerOpenGlRenderingViewRenderer.cc \
	NucleusMarkerOpenGlRenderingViewRenderer.cc \
	SHSurfaceMarkerOpenGlRenderingViewRenderer.cc \
	SurfaceMarkerOpenGlRenderingViewRenderer.cc \
	CellMarkerOpenGlRenderingViewRenderer.cc \
	CylinderMarkerOpenGlRenderingViewRenderer.cc \
	MultiChannelModel.cc \
	HDF5SelectionDialog.cc \
	SVGDataIO.cc \
	AnalyzeAndSTLDataIO.cc \
	HDF5DataIO.cc \
	CSVDataIO.cc \
	LabellingMainWidget.cc \
	ExceptionAwareApplication.cc \
	OrthoViewOverlay.cc \
	ColorMap.cc \
	IColorMapEditor.cc \
	ColorMapEditorWidget.cc \
	liblabelling_qt4.qrc

nodist_libLabelling_qt4_la_SOURCES = \
	$(MOC_GEN_SOURCES)

moc_%.cc: %.hh
	$(MOC) $< -o $@

%.rcc: %.qrc
	$(RCC) -binary $< -o $@

MOSTLYCLEANFILES = $(MOC_GEN_SOURCES)
