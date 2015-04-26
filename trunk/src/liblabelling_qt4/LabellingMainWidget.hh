/**************************************************************************
 *
 * This file belongs to the iRoCS Toolbox.
 *
 * Copyright (C) 2015 Thorsten Falk
 *
 *        Image Analysis Lab, University of Freiburg, Germany
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 **************************************************************************/

#ifndef LABELLINGMAINWIDGET_HH
#define LABELLINGMAINWIDGET_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

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
  void setPluginFolder();
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

  QList<QAction*> _plugins;
  std::list<PluginInterface*> _pluginsRunning;

  QProgressBar *p_progressBar;
  iRoCS::ProgressReporterQt4 *p_progressReporter;
  
};

#endif
