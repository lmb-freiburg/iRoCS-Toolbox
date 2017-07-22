set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(L_T_C_SOURCES
  SelectDlg.cc mainFrame.cc limagelabel.cc lviewer.cc lviewermainwindow.cc
  L_T_C.cc L_T_C.qrc)

set(L_T_C_HEADERS
  SelectDlg.hh defaultParameters.hh mainFrame.hh lblitzDataExplorer.hh
  limagelabel.hh lviewer.hh lviewermainwindow.hh)

set(L_T_C_RESOURCES
  images/ad.png images/gundam-exia.png images/mw.png images/new.png
  images/print.png images/save.png images/undo.png)

add_executable(L_T_C ${L_T_C_SOURCES} ${L_T_C_HEADERS} ${L_T_C_RESOURCES})
target_link_libraries(L_T_C
  LINK_PUBLIC ArrayToolbox Qt4::QtCore Qt4::QtGui)

install(TARGETS L_T_C RUNTIME DESTINATION bin)

set(CMAKE_AUTORCC OFF)
set(CMAKE_AUTOMOC OFF)
