/**************************************************************************
**       Title: Main Widget for the iRoCS Toolbox
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    Labelling Main Window
**
**************************************************************************/

#ifndef LABELLINGMAINWIDGET_HH
#define LABELLINGMAINWIDGET_HH

#include <QtGui/QMainWindow>
#include <QtCore/QMap>

#include "StreamBufTextEditQt4.hh"
#include <libProgressReporter/ProgressReporterQt4.hh>

class QDockWidget;
class QTextEdit;

class MultiChannelModel;
class OrthoViewWidget;
class OpenGlRenderingViewWidget;
class PluginInterface;

#include "Marker.hh"

class LabellingMainWidget : public QMainWindow
{
  
  Q_OBJECT

public:
  
  static std::string const __labellingVersionString;

  LabellingMainWidget(
      size_t memoryLimit, std::string const &fileName = "",
      bool forceImport = false, QWidget* parent = 0);
  ~LabellingMainWidget();
  
  MultiChannelModel* multiChannelModel();
  OrthoViewWidget* orthoView();
  OpenGlRenderingViewWidget* openGlRenderingView();

  const std::string& fileName() const;

  void runPlugin(
      QString const &name, std::map<std::string,std::string> const &parameters);

  bool closeProjectInteractive();

  iRoCS::ProgressReporterQt4 *progressReporter();

public slots:

  void open();
  void save();
  void saveAs();
  void exportToSVG();
  void exportToAnalyzeAndSTL();
  void closeProject();
  void createAnnotationChannel();
  void createAnnotationChannel(Marker::MarkerType mtype);
  void importChannels();
  void importCSVMarkers();
  void exportCSVMarkers();
  void showChannelControlWidget(bool show);
  void showInfoWidget(bool show);
  void showOpenGlWidget(bool show);
  void showLogWidget(bool show);
  void showDebugWidget(bool show);
  void setInterpolation(bool interpolation);
  void snapToVoxelGrid(bool snap);
  void showHelperLines(bool show);
  void runPlugin(QAction* action);

  void about();

  void setUserInteractionEnabled(bool enable);

  void showError(const QString& message);

  size_t memoryLimit() const;

private:

  void searchPlugins();

  std::string getOpenFileName();
  std::string getSaveFileName();

  void closeEvent(QCloseEvent*);
  void resizeEvent(QResizeEvent*);

  std::string _initialFileName;

  size_t _memoryLimit;
  std::string _fileName;

  QMenu* p_fileMenu;
  QMenu* p_channelMenu;
  QMenu* p_viewMenu;
  QMenu* p_pluginMenu;
  QMenu* p_helpMenu;

  MultiChannelModel *p_multiChannelModel;
  OrthoViewWidget *p_orthoView;
  OpenGlRenderingViewWidget *p_openGlView;

  QDockWidget* p_channelControlDockWidget;
  QDockWidget* p_infoDockWidget;

  StreamBufTextEditQt4* p_logBuffer;
  std::streambuf *p_origCoutBuffer;
  QDockWidget* p_logWidget;
  QTextEdit* p_logTextEdit;

  StreamBufTextEditQt4* p_debugBuffer;
  std::streambuf *p_origCerrBuffer;
  QDockWidget* p_debugWidget;
  QTextEdit* p_debugTextEdit;

  QMap<QString, QString> _plugins;
  std::list<PluginInterface*> _pluginsRunning;

  QProgressBar *p_progressBar;
  iRoCS::ProgressReporterQt4 *p_progressReporter;
  
};

#endif
