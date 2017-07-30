set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(iRoCS_GUI_HEADERS
  OrthoviewWidget.hh OrthoviewPlane.hh OrthoviewRenderer.hh
  MultichannelModelTreeWidget.hh MultichannelModelControlWidgetStack.hh
  ChannelOrthoviewRenderer.hh ChannelControlWidget.hh ChannelTreeWidgetItem.hh
  LineEditDelegate.hh DoubleSpinBoxDelegate.hh FloatColormapBarWidget.hh
  FloatColormapEditorWidget.hh FloatChannelTreeWidgetItem.hh
  FloatChannelOrthoviewRenderer.hh FloatChannelControlWidget.hh
  StreamBufTextEditQt4.hh HDF5SelectionDialog.hh ControlElement.hh
  Double3ControlElement.hh BoolControlElement.hh DoubleControlElement.hh
  DoubleRangeControlElement.hh ColorControlElement.hh StringControlElement.hh
  Double4x4ControlElement.hh iRoCSMainWindow.hh)
  
set(iRoCS_GUI_SOURCES
  OrthoviewWidget.cc OrthoviewPlane.cc OrthoviewRenderer.cc
  MultichannelModelTreeWidget.cc MultichannelModelControlWidgetStack.cc
  ChannelOrthoviewRenderer.cc ChannelControlWidget.cc ChannelTreeWidgetItem.cc
  LineEditDelegate.cc DoubleSpinBoxDelegate.cc FloatColormapBarWidget.cc
  FloatColormapEditorWidget.cc FloatChannelTreeWidgetItem.cc
  FloatChannelOrthoviewRenderer.cc FloatChannelControlWidget.cc
  StreamBufTextEditQt4.cc HDF5SelectionDialog.cc ControlElement.cc
  Double3ControlElement.cc BoolControlElement.cc DoubleControlElement.cc
  DoubleRangeControlElement.cc ColorControlElement.cc StringControlElement.cc
  Double4x4ControlElement.cc iRoCSMainWindow.cc iRoCS_GUI.cc)

set(iRoCS_GUI_RESOURCES
  iRoCS_GUI.qrc icons/iRoCSToolboxIcon.svg icons/show-scalebar.svg
  icons/oxygen/32x32/actions/document-properties.png
  icons/oxygen/32x32/actions/edit-delete.png
  icons/oxygen/32x32/actions/process-stop.png
  icons/oxygen/32x32/actions/zoom-in.png
  icons/oxygen/32x32/actions/zoom-original.png
  icons/oxygen/32x32/actions/zoom-out.png)
  
add_executable(iRoCS_GUI
  ${iRoCS_GUI_SOURCES} ${iRoCS_GUI_HEADERS} ${iRoCS_GUI_RESOURCES})

target_link_libraries(iRoCS_GUI
  PRIVATE MultiChannelModel cmdline ProgressReporter_qt4 Qt4::QtGui)

# generate proper GUI program on Windows
if(WIN32 AND MSVC)
  set_target_properties(iRoCS_GUI PROPERTIES WIN32_EXECUTABLE YES
    LINK_FLAGS "/ENTRY:mainCRTStartup")
endif()

install(TARGETS iRoCS_GUI RUNTIME DESTINATION bin)

set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC OFF)
