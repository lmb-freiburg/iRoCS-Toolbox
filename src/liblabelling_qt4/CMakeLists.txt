set(Labelling_qt4_VERSION_MAJOR 4)
set(Labelling_qt4_VERSION_MINOR 0)
set(Labelling_qt4_VERSION_PATCH 0)
set(Labelling_qt4_VERSION ${Labelling_qt4_VERSION_MAJOR}.${Labelling_qt4_VERSION_MINOR}.${Labelling_qt4_VERSION_PATCH})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(Labelling_qt4_SOURCES
  StreamBufTextEditQt4.cc AbstractControlElement.cc BoolControlElement.cc
  IntControlElement.cc Int3ControlElement.cc IntVectorControlElement.cc
  DoubleControlElement.cc Double3ControlElement.cc DoubleVectorControlElement.cc
  Double4x4ControlElement.cc DoubleRangeControlElement.cc
  StringControlElement.cc StringSelectionControlElement.cc
  IntDoubleMapControlElement.cc ChannelSelectionControlElement.cc
  FileNameSelectionControlElement.cc HDF5SelectionControlElement.cc
  ColorControlElement.cc Marker.cc MarkerPresetWidget.cc MarkerControlWidget.cc
  PointMarker.cc SphereMarker.cc SHSurfaceMarker.cc NucleusMarker.cc
  CylinderMarker.cc SurfaceMarker.cc CellMarker.cc UserInteractionEvent.cc
  OrthoViewUserInteractionEvent.cc ChannelSpecs.cc DataChannelSpecs.cc
  RGBChannelSpecs.cc VisualizationChannelSpecs.cc AnnotationChannelSpecs.cc
  SplineCurveChannelSpecs.cc IRoCSChannelSpecs.cc IRoCSSCTChannelSpecs.cc
  ViewWidget.cc MarkerRenderer.cc ChannelSpecsRenderer.cc OrthoViewPlane.cc
  OrthoViewWidget.cc ChannelSpecsOrthoViewRenderer.cc MarkerOrthoViewRenderer.cc
  PointMarkerOrthoViewRenderer.cc SphereMarkerOrthoViewRenderer.cc
  SHSurfaceMarkerOrthoViewRenderer.cc NucleusMarkerOrthoViewRenderer.cc
  CylinderMarkerOrthoViewRenderer.cc SurfaceMarkerOrthoViewRenderer.cc
  CellMarkerOrthoViewRenderer.cc DataChannelSpecsOrthoViewRenderer.cc
  RGBChannelSpecsOrthoViewRenderer.cc
  VisualizationChannelSpecsOrthoViewRenderer.cc
  AnnotationChannelSpecsOrthoViewRenderer.cc
  SplineCurveChannelSpecsOrthoViewRenderer.cc
  IRoCSChannelSpecsOrthoViewRenderer.cc IRoCSSCTChannelSpecsOrthoViewRenderer.cc
  OpenGlRenderingWidget.cc OpenGlRenderingViewWidget.cc
  OpenGlRenderingSettingsWidget.cc ChannelSpecsOpenGlRenderingViewRenderer.cc
  DataChannelSpecsOpenGlRenderingViewRenderer.cc
  RGBChannelSpecsOpenGlRenderingViewRenderer.cc
  VisualizationChannelSpecsOpenGlRenderingViewRenderer.cc
  AnnotationChannelSpecsOpenGlRenderingViewRenderer.cc
  SplineCurveChannelSpecsOpenGlRenderingViewRenderer.cc
  IRoCSChannelSpecsOpenGlRenderingViewRenderer.cc
  IRoCSSCTChannelSpecsOpenGlRenderingViewRenderer.cc
  MarkerOpenGlRenderingViewRenderer.cc PointMarkerOpenGlRenderingViewRenderer.cc
  SphereMarkerOpenGlRenderingViewRenderer.cc
  NucleusMarkerOpenGlRenderingViewRenderer.cc
  SHSurfaceMarkerOpenGlRenderingViewRenderer.cc
  SurfaceMarkerOpenGlRenderingViewRenderer.cc
  CellMarkerOpenGlRenderingViewRenderer.cc
  CylinderMarkerOpenGlRenderingViewRenderer.cc MultiChannelModel.cc
  HDF5SelectionDialog.cc SVGDataIO.cc AnalyzeAndSTLDataIO.cc HDF5DataIO.cc
  CSVDataIO.cc LabellingMainWidget.cc ExceptionAwareApplication.cc
  OrthoViewOverlay.cc ColorMap.cc IColorMapEditor.cc ColorMapEditorWidget.cc
  liblabelling_qt4.qrc )

set(Labelling_qt4_HEADERS
  StreamBufTextEditQt4.hh AbstractControlElement.hh BoolControlElement.hh
  IntControlElement.hh Int3ControlElement.hh IntVectorControlElement.hh
  DoubleControlElement.hh Double3ControlElement.hh DoubleVectorControlElement.hh
  Double4x4ControlElement.hh DoubleRangeControlElement.hh
  StringControlElement.hh StringSelectionControlElement.hh
  IntDoubleMapControlElement.hh ChannelSelectionControlElement.hh
  FileNameSelectionControlElement.hh HDF5SelectionControlElement.hh
  ColorControlElement.hh ColorMap.hh IColorMapEditor.hh
  ColorMapEditorWidget.hh UserInteractionEvent.hh
  OrthoViewUserInteractionEvent.hh Marker.hh MarkerPresetWidget.hh
  MarkerControlWidget.hh PointMarker.hh SphereMarker.hh SHSurfaceMarker.hh
  NucleusMarker.hh CylinderMarker.hh SurfaceMarker.hh CellMarker.hh
  ChannelSpecs.hh DataChannelSpecs.hh RGBChannelSpecs.hh
  VisualizationChannelSpecs.hh AnnotationChannelSpecs.hh
  SplineCurveChannelSpecs.hh IRoCSChannelSpecs.hh IRoCSSCTChannelSpecs.hh
  ViewWidget.hh MarkerRenderer.hh ChannelSpecsRenderer.hh OrthoViewPlane.hh
  OrthoViewWidget.hh ChannelSpecsOrthoViewRenderer.hh MarkerOrthoViewRenderer.hh
  PointMarkerOrthoViewRenderer.hh SphereMarkerOrthoViewRenderer.hh
  SHSurfaceMarkerOrthoViewRenderer.hh NucleusMarkerOrthoViewRenderer.hh
  CylinderMarkerOrthoViewRenderer.hh SurfaceMarkerOrthoViewRenderer.hh
  CellMarkerOrthoViewRenderer.hh DataChannelSpecsOrthoViewRenderer.hh
  RGBChannelSpecsOrthoViewRenderer.hh
  VisualizationChannelSpecsOrthoViewRenderer.hh
  AnnotationChannelSpecsOrthoViewRenderer.hh
  SplineCurveChannelSpecsOrthoViewRenderer.hh
  IRoCSChannelSpecsOrthoViewRenderer.hh IRoCSSCTChannelSpecsOrthoViewRenderer.hh
  OpenGlRenderingWidget.hh OpenGlRenderingViewWidget.hh
  OpenGlRenderingSettingsWidget.hh ChannelSpecsOpenGlRenderingViewRenderer.hh
  DataChannelSpecsOpenGlRenderingViewRenderer.hh
  RGBChannelSpecsOpenGlRenderingViewRenderer.hh
  VisualizationChannelSpecsOpenGlRenderingViewRenderer.hh
  AnnotationChannelSpecsOpenGlRenderingViewRenderer.hh
  SplineCurveChannelSpecsOpenGlRenderingViewRenderer.hh
  IRoCSChannelSpecsOpenGlRenderingViewRenderer.hh
  IRoCSSCTChannelSpecsOpenGlRenderingViewRenderer.hh
  MarkerOpenGlRenderingViewRenderer.hh
  PointMarkerOpenGlRenderingViewRenderer.hh
  SphereMarkerOpenGlRenderingViewRenderer.hh
  NucleusMarkerOpenGlRenderingViewRenderer.hh
  SHSurfaceMarkerOpenGlRenderingViewRenderer.hh
  SurfaceMarkerOpenGlRenderingViewRenderer.hh
  CellMarkerOpenGlRenderingViewRenderer.hh
  CylinderMarkerOpenGlRenderingViewRenderer.hh MultiChannelModel.hh
  HDF5SelectionDialog.hh SVGDataIO.hh AnalyzeAndSTLDataIO.hh HDF5DataIO.hh
  CSVDataIO.hh LabellingMainWidget.hh OrthoViewOverlay.hh
  ExceptionAwareApplication.hh PluginInterface.hh)

set(Labelling_qt4_RESOURCES
  icons/labellingIcon.svg icons/oxygen/32x32/actions/document-properties.png
  icons/oxygen/32x32/actions/edit-delete.png
  icons/oxygen/32x32/actions/process-stop.png
  icons/oxygen/32x32/actions/zoom-in.png
  icons/oxygen/32x32/actions/zoom-original.png
  icons/oxygen/32x32/actions/zoom-out.png)

if (BUILD_SHARED_LIBS OR BUILD_STATIC_LIBS)
  # Install development headers
  install(FILES ${Labelling_qt4_HEADERS}
    DESTINATION include/${PACKAGE}/libLabelling_qt4)
endif()

# Build shared library if requested
if (BUILD_SHARED_LIBS)
  add_library(Labelling_qt4 SHARED
    ${Labelling_qt4_SOURCES} ${Labelling_qt4_HEADERS}
    ${Labelling_qt4_RESOURCES})
  set_target_properties(Labelling_qt4 PROPERTIES
    VERSION ${Labelling_qt4_VERSION} SOVERSION ${Labelling_qt4_VERSION_MAJOR})
  target_include_directories(Labelling_qt4 PUBLIC ${OPENGL_INCLUDE_DIRS})
  target_link_libraries(Labelling_qt4
    PUBLIC segmentation ProgressReporter_qt4 BlitzAnalyze Qt4::QtCore
    Qt4::QtOpenGL Qt4::QtXml Qt4::QtSvg ${OPENGL_LIBRARIES})
  if(WIN32 AND BUILD_SHARED_LIBS)
    target_compile_definitions(Labelling_qt4 INTERFACE -DLabelling_qt4_USE_DLL)
  endif()
  install(TARGETS Labelling_qt4
    EXPORT iRoCS-ToolboxTargets
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include/${PACKAGE}/libLabelling_qt4)
endif()

# Build static library if requested
if(BUILD_STATIC_LIBS)
  add_library(Labelling_qt4_static STATIC
    ${Labelling_qt4_SOURCES} ${Labelling_qt4_HEADERS}
    ${Labelling_qt4_RESOURCES})
  target_include_directories(Labelling_qt4_static
    PUBLIC ${OPENGL_INCLUDE_DIRS})
  target_link_libraries(Labelling_qt4_static
    PUBLIC segmentation_static ProgressReporter_qt4_static BlitzAnalyze_static
    Qt4::QtCore Qt4::QtOpenGL Qt4::QtXml Qt4::QtSvg ${OPENGL_LIBRARIES})
  install(TARGETS Labelling_qt4_static
    EXPORT iRoCS-ToolboxTargets
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include/${PACKAGE}/libLabelling_qt4)

  # On Linux rename static library to match shared library name
  if(UNIX)
    SET_TARGET_PROPERTIES(Labelling_qt4_static PROPERTIES
      OUTPUT_NAME Labelling_qt4)
  endif()
endif()

set(CMAKE_AUTORCC OFF)
set(CMAKE_AUTOMOC OFF)
