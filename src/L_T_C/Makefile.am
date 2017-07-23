bin_PROGRAMS = L_T_C
lbldatadir = $(pkgdatadir)

MOC = @QT4_MOC@
RCC = @QT4_RCC@

AM_CPPFLAGS = -I$(top_srcdir)/src $(BLITZ_CFLAGS) $(HDF5_CFLAGS) \
	$(FFTW_CFLAGS) $(GSL_CFLAGS) $(OPENCV_CFLAGS) $(QT4_CFLAGS)
AM_CXXFLAGS = -Wno-long-long
AM_LDFLAGS = $(NONCONSOLE_LDFLAGS)

L_T_C_LDADD = \
	$(top_builddir)/src/libArrayToolbox/libArrayToolbox.la \
	$(top_builddir)/src/libBlitzFFTW/libBlitzFFTW.la \
	$(top_builddir)/src/libBlitzHdf5/libBlitzHdf5.la \
	$(top_builddir)/src/libProgressReporter/libProgressReporter.la \
	$(top_builddir)/src/libBaseFunctions/libBaseFunctions.la \
	$(BLITZ_LIBS) $(HDF5_LIBS) $(FFTW_LIBS) $(GSL_LIBS) $(OPENCV_LIBS) \
	$(QT4_LIBS)

lbldata_DATA = L_T_C.rcc

EXTRA_DIST = images/ad.png images/gundam-exia.png images/mw.png \
  images/new.png images/print.png images/save.png images/undo.png

MOC_GEN_SOURCES = moc_SelectDlg.cc moc_mainFrame.cc moc_limagelabel.cc \
  moc_lviewer.cc moc_lviewermainwindow.cc

noinst_HEADERS = SelectDlg.hh defaultParameters.hh mainFrame.hh \
  lblitzDataExplorer.hh limagelabel.hh lviewer.hh lviewermainwindow.hh

L_T_C_SOURCES = SelectDlg.cc mainFrame.cc limagelabel.cc lviewer.cc \
  lviewermainwindow.cc L_T_C.cc L_T_C.qrc

nodist_L_T_C_SOURCES = $(MOC_GEN_SOURCES)

moc_%.cc: %.hh
	$(MOC) $< -o $@

%.rcc: %.qrc
	$(RCC) -binary $< -o $@

MOSTLYCLEANFILES = $(MOC_GEN_SOURCES)
