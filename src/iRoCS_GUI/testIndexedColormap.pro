HEADERS = \
  ControlElement.hh \
  DoubleControlElement.hh \
  ColorControlElement.hh \
  BoolControlElement.hh \
  Observable.hh \
  IndexedColormap.hh \
  IndexedColormapObserver.hh \
  IndexedColormapBarWidget.hh \
  IndexedColormapEditorWidget.hh

SOURCES = \
  ControlElement.cc \
  DoubleControlElement.cc \
  ColorControlElement.cc \
  BoolControlElement.cc \
  Observable.cc \
  IndexedColormap.cc \
  IndexedColormapObserver.cc \
  IndexedColormapBarWidget.cc \
  IndexedColormapEditorWidget.cc \
  testIndexedColormap.cc

CODECFORTR = UTF-8
CODECFORSRC = UTF-8

RESOURCES = iRoCSToolbox.qrc

DEFINES += HAVE_CONFIG_H DEBUG_VERBOSE_XML
QMAKE_CXXFLAGS_DEBUG += -fopenmp -O2 -Wextra -pedantic -Wno-long-long
QMAKE_CXXFLAGS_RELEASE += -fopenmp -Wextra -pedantic -Wno-long-long
INCLUDEPATH = $$(HOME)/software/$$(HOSTGCC)/include/irocs-toolbox /usr/include/hdf5/serial
LIBS += -L$$(HOME)/software/$$(HOSTGCC)/lib \
  -lArrayToolbox -lProgressReporter_qt4 -lProgressReporter -lBlitzHdf5 -lBlitzFFTW -lBaseFunctions -lcmdline -lhdf5_serial -lblitz -lgsl -lm -lz -lgomp

QT += svg

CONFIG += qt debug
