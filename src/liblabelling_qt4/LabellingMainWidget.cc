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

#include "LabellingMainWidget.hh"

#include <QtGui/QDockWidget>
#include <QtGui/QTextEdit>
#include <QtCore/QCoreApplication>
#include <QtCore/QSettings>
#include <QtGui/QApplication>
#include <QtGui/QMenuBar>
#include <QtGui/QMenu>
#include <QtGui/QStatusBar>
#include <QtGui/QFileDialog>
#include <QtGui/QVBoxLayout>
#include <QtGui/QMessageBox>
#include <QtGui/QInputDialog>
#include <QtGui/QCloseEvent>
#include <QtGui/QToolButton>

// Resources
#include <QtCore/QResource>

// Plugin handling
#include <QtCore/QDir>
#include <QtCore/QPluginLoader>

#include "MultiChannelModel.hh"
#include "OrthoViewWidget.hh"
#include "OpenGlRenderingViewWidget.hh"
#include "PluginInterface.hh"
#include "DataChannelSpecs.hh"
#include "AnnotationChannelSpecs.hh"
#include "PointMarker.hh"
#include "SphereMarker.hh"
#include "SVGDataIO.hh"
#include "AnalyzeAndSTLDataIO.hh"
#include "HDF5DataIO.hh"
#include "CSVDataIO.hh"
#include "ColorMapEditorWidget.hh"

#ifdef DEBUG
std::string const LabellingMainWidget::__labellingVersionString =
    std::string("iRoCS Toolbox rev. " + std::string(VERSIONSTRING) +
                " debug last build: ") + __DATE__ + " " + __TIME__;
#else
std::string const LabellingMainWidget::__labellingVersionString =
    std::string("iRoCS Toolbox rev. " + std::string(VERSIONSTRING) +
                " last build: ") + __DATE__ + " " + __TIME__;
#endif

LabellingMainWidget::LabellingMainWidget(
    size_t memoryLimit, std::string const &fileName, bool forceImport,
    QWidget* parent)
        : QMainWindow(parent), _initialFileName(fileName),
          _memoryLimit(memoryLimit), _fileName(fileName)
{
  QCoreApplication::setOrganizationName("LMB");
  QCoreApplication::setOrganizationDomain("lmb.informatik.uni-freiburg.de");
  QCoreApplication::setApplicationName("IRoCS Toolbox");
  QString resourceDir =
      QCoreApplication::applicationDirPath() + "/../share/irocs-toolbox";
  QResource::registerResource(resourceDir + "/liblabelling_qt4.rcc");
  setWindowIcon(QIcon(":/labellingIcon.svg"));
  QSettings settings;
  resize(settings.value("MainWindow/size", QSize(800, 600)).toSize());
  setObjectName("labelling|MainWidget");

  setWindowTitle("iRoCS Toolbox");

  p_logWidget = new QDockWidget("Log", this);
  p_logTextEdit = new QTextEdit;
  p_logTextEdit->setReadOnly(true);
  p_logBuffer = new StreamBufTextEditQt4(p_logTextEdit);
  p_origCoutBuffer = std::cout.rdbuf(p_logBuffer);
  p_logWidget->setWidget(p_logTextEdit);
  addDockWidget(Qt::BottomDockWidgetArea, p_logWidget);
  p_logWidget->setVisible(false);
  p_logWidget->resize(
      settings.value("LogWindow/size", QSize(400, 400)).toSize());

  p_debugWidget = new QDockWidget("Warnings", this);
  p_debugTextEdit = new QTextEdit;
  p_debugTextEdit->setReadOnly(true);
  p_debugBuffer = new StreamBufTextEditQt4(p_debugTextEdit);
#ifndef DEBUG
  p_origCerrBuffer = std::cerr.rdbuf(p_debugBuffer);
#endif
  p_debugWidget->setWidget(p_debugTextEdit);
  addDockWidget(Qt::BottomDockWidgetArea, p_debugWidget);
  p_debugWidget->setVisible(false);
  p_debugWidget->resize(
      settings.value("DebugWindow/size", QSize(400, 400)).toSize());

  p_multiChannelModel = new MultiChannelModel();
  p_orthoView = new OrthoViewWidget(p_multiChannelModel);  
  p_openGlView = new OpenGlRenderingViewWidget(
      p_multiChannelModel, this, Qt::Window);
  p_openGlView->setWindowTitle(tr("3D Rendering"));
  p_openGlView->resize(
      settings.value("OpenGlWidget/size", QSize(500, 400)).toSize());
  p_openGlView->setVisible(false);
  setCentralWidget(p_orthoView);
  
  p_channelControlDockWidget = new QDockWidget(tr("Channel Control"), this);
  p_channelControlDockWidget->setWidget(
      p_multiChannelModel->channelEditorWidget());
  addDockWidget(Qt::RightDockWidgetArea, p_channelControlDockWidget);
  p_channelControlDockWidget->resize(
      settings.value(
          "ChannelControlDock/size",
          p_multiChannelModel->channelEditorWidget()->minimumSize()).toSize());

  p_infoDockWidget = new QDockWidget(tr("Information"), this);
  p_infoDockWidget->setWidget(p_multiChannelModel->infoWidget());
  addDockWidget(Qt::RightDockWidgetArea, p_infoDockWidget);
  p_infoDockWidget->resize(
      settings.value(
          "InfoDock/size",
          p_multiChannelModel->infoWidget()->minimumSize()).toSize());

  p_fileMenu = new QMenu(tr("&File"));
  QAction *fileOpenAction =
      p_fileMenu->addAction(tr("&Open..."), this, SLOT(open()),
                            QKeySequence(tr("Ctrl+O", "File|Open")));
  QAction* fileSaveAction =
      p_fileMenu->addAction(tr("&Save"), this, SLOT(save()),
                            QKeySequence(tr("Ctrl+S", "File|Save")));
  fileSaveAction->setEnabled(false);
  connect(p_multiChannelModel, SIGNAL(modifiedFlagChanged(bool)),
          fileSaveAction, SLOT(setEnabled(bool)));
  QAction* fileSaveAsAction =
      p_fileMenu->addAction(tr("Save as..."), this, SLOT(saveAs()));
  fileSaveAsAction->setEnabled(false);
  connect(p_multiChannelModel, SIGNAL(isEmpty(bool)), fileSaveAsAction,
          SLOT(setDisabled(bool)));
  QAction* exportToSVGAction =
      p_fileMenu->addAction(tr("Export as Scalable Vector Graphics (SVG)"));
  connect(p_multiChannelModel, SIGNAL(isEmpty(bool)), exportToSVGAction,
          SLOT(setDisabled(bool)));
  connect(exportToSVGAction, SIGNAL(activated()), SLOT(exportToSVG()));
  QAction* exportToAnalyzeAndSTLAction =
      p_fileMenu->addAction(tr("Export as Analyze and STL"));
  connect(p_multiChannelModel, SIGNAL(isEmpty(bool)),
          exportToAnalyzeAndSTLAction, SLOT(setDisabled(bool)));
  connect(exportToAnalyzeAndSTLAction, SIGNAL(activated()),
          SLOT(exportToAnalyzeAndSTL()));
  QAction* fileCloseAction =
      p_fileMenu->addAction(tr("Close"), this, SLOT(closeProject()));
  fileCloseAction->setEnabled(false);
  connect(p_multiChannelModel, SIGNAL(isEmpty(bool)), fileCloseAction,
          SLOT(setDisabled(bool)));
  p_fileMenu->addSeparator();
  p_fileMenu->addAction(tr("E&xit"), this, SLOT(close()),
                        QKeySequence(tr("Ctrl+X", "File|Exit")));
  menuBar()->addMenu(p_fileMenu);

  p_channelMenu = new QMenu(tr("&Channel"));
  p_channelMenu->addAction(tr("New Annotation Channel"), this,
                           SLOT(createAnnotationChannel()));
  p_channelMenu->addSeparator();
  QAction *importChannelsAction =
      p_channelMenu->addAction(tr("Import Channels..."), this,
                               SLOT(importChannels()));

  p_channelMenu->addSeparator();
  p_channelMenu->addAction(tr("Import annotation channel from CSV..."), this,
                           SLOT(importCSVMarkers()));
  QAction *exportCSVAction =
      p_channelMenu->addAction(tr("Export annotation channel to CSV..."), this,
                               SLOT(exportCSVMarkers()));  
  connect(p_multiChannelModel, SIGNAL(annotationChannelSelected(bool)),
          exportCSVAction, SLOT(setEnabled(bool)));

  menuBar()->addMenu(p_channelMenu);

  p_viewMenu = new QMenu(tr("&View"));
  p_viewMenu->addAction(p_channelControlDockWidget->toggleViewAction());
  p_viewMenu->addAction(p_infoDockWidget->toggleViewAction());
  QAction* showOpenGlViewAction =
      p_viewMenu->addAction(tr("Show OpenGL Rendering"));
  showOpenGlViewAction->setCheckable(true);
  showOpenGlViewAction->setChecked(false);  
  connect(showOpenGlViewAction, SIGNAL(toggled(bool)),
          SLOT(showOpenGlWidget(bool)));
  connect(p_openGlView, SIGNAL(visibilityStateChanged(bool)),
          showOpenGlViewAction, SLOT(setChecked(bool)));
  p_viewMenu->addAction(p_logWidget->toggleViewAction());
  p_viewMenu->addAction(p_debugWidget->toggleViewAction());
  p_viewMenu->addSeparator();
  QAction* interpolateAction =
      p_viewMenu->addAction(tr("Bilinear Interpolation"));
  interpolateAction->setCheckable(true);
  connect(interpolateAction, SIGNAL(toggled(bool)),
          SLOT(setInterpolation(bool)));
  interpolateAction->setChecked(false);
  QAction* snapToVoxelGridAction =
      p_viewMenu->addAction(tr("Snap To Voxel Grid"));
  snapToVoxelGridAction->setCheckable(true);
  connect(snapToVoxelGridAction, SIGNAL(toggled(bool)),
          SLOT(snapToVoxelGrid(bool)));
  snapToVoxelGridAction->setChecked(true);
  QAction* viewHelperLinesAction =
      p_viewMenu->addAction(tr("View Helper Lines"));
  viewHelperLinesAction->setCheckable(true);
  connect(viewHelperLinesAction, SIGNAL(toggled(bool)),
          SLOT(showHelperLines(bool)));
  viewHelperLinesAction->setChecked(false);
  menuBar()->addMenu(p_viewMenu);

  p_pluginMenu = new QMenu(tr("&Plugins"));
  QAction *setPluginFolderAction =
      p_pluginMenu->addAction(tr("Set Plugin Folder..."));
  connect(setPluginFolderAction, SIGNAL(triggered()), SLOT(setPluginFolder()));
  p_pluginMenu->addSeparator();
  searchPlugins();
  menuBar()->addMenu(p_pluginMenu);
  connect(p_pluginMenu, SIGNAL(triggered(QAction*)), SLOT(runPlugin(QAction*)));
  
  p_helpMenu = new QMenu(tr("&Help"));
  p_helpMenu->addAction(tr("About"), this, SLOT(about()));
  p_helpMenu->addAction(tr("About Qt"), qApp, SLOT(aboutQt()));
  menuBar()->addMenu(p_helpMenu);

  QLabel *statusText = new QLabel;
  statusBar()->addWidget(statusText);
  p_progressBar = new QProgressBar;
  p_progressBar->setFixedWidth(width() / 4);
  p_progressBar->setVisible(false);
  statusBar()->addPermanentWidget(p_progressBar);
  QToolButton *cancelButton = new QToolButton;
  cancelButton->setIcon(
      QIcon::fromTheme("process-stop", QIcon(":/process-stop.png")));
  cancelButton->setText(tr("cancel"));
  cancelButton->setToolTip(tr("Cancel current operation"));
  cancelButton->setVisible(false);
  statusBar()->addPermanentWidget(cancelButton);
  p_progressReporter = new iRoCS::ProgressReporterQt4(
      p_progressBar, NULL, statusText, cancelButton, &std::cout);

  show();

  if (fileName != "")
  {
    bool isLabellingProject = false;
    try
    {
      {
        BlitzH5File inFile(fileName);
        isLabellingProject = inFile.existsGroup(".labelling_qt4");
      }
      if (!forceImport && isLabellingProject) fileOpenAction->trigger();
      else importChannelsAction->trigger();
    }
    catch (BlitzH5Error &e)
    {
      showError(("Could not open '" + fileName + "' for reading: " +
                 e.what()).c_str());
    }
  }
}

LabellingMainWidget::~LabellingMainWidget()
{
  QSettings settings;
  settings.setValue("MainWindow/size", size());
  settings.setValue("ChannelControlDock/size",
                    p_channelControlDockWidget->size());
  settings.setValue("InfoDock/size", p_infoDockWidget->size());
  settings.setValue("LogWindow/size", p_logWidget->size());
  settings.setValue("DebugWindow/size", p_debugWidget->size());
  settings.setValue("OpenGlWidget/size", p_openGlView->size());

  menuBar()->clear();
  delete p_fileMenu;
  delete p_channelMenu;
  std::cout.rdbuf(p_origCoutBuffer);
  delete p_logBuffer;
#ifndef DEBUG
  std::cerr.rdbuf(p_origCerrBuffer);
#endif
  delete p_debugBuffer;
}

MultiChannelModel* LabellingMainWidget::multiChannelModel()
{
  return p_multiChannelModel;
}

OrthoViewWidget* LabellingMainWidget::orthoView()
{
  return p_orthoView;
}

OpenGlRenderingViewWidget* LabellingMainWidget::openGlRenderingView()
{
  return p_openGlView;
}

const std::string& LabellingMainWidget::fileName() const
{
  return _fileName;
}

bool LabellingMainWidget::closeProjectInteractive()
{
  if (p_multiChannelModel->modified())
  {
    QMessageBox::StandardButton res = QMessageBox::question(
        this, tr("Save model"),
        tr("The current model has unsaved data, do you want to save it?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
        QMessageBox::Save);
    switch (res)
    {
    case QMessageBox::Save:
      save();
      if (p_multiChannelModel->modified()) return false;
      break;
    case QMessageBox::Discard:
      break;
    default:
      return false;
      break;
    }
  }
  while (p_multiChannelModel->nChannels() != 0)
      p_multiChannelModel->removeChannel(*(--p_multiChannelModel->end()));
  p_multiChannelModel->setModified(false);
  _fileName = "";
  setWindowTitle("IRoCS Toolbox");
  p_orthoView->update();
  p_openGlView->update();
  return true;
}

void LabellingMainWidget::open() 
{
  if (_initialFileName == "")
  {
    if (!closeProjectInteractive()) return;
    _fileName = getOpenFileName();
    if (_fileName == "") return;
  }
  else
  {
    _fileName = _initialFileName;
    _initialFileName = "";
  }
  HDF5DataIO dataLoader(this, p_multiChannelModel, _fileName, HDF5DataIO::Open);
  if (dataLoader.run() != HDF5DataIO::Ok)
  {
    _fileName = "";
    setWindowTitle("IRoCS Toolbox");
  }
  else setWindowTitle(("IRoCS Toolbox: " + _fileName).c_str());
  p_multiChannelModel->setModified(false);
}

void LabellingMainWidget::save() 
{
  std::string oldFileName = _fileName;
  if (_fileName == "") _fileName = getSaveFileName();
  if (_fileName == "") return;
  HDF5DataIO dataSaver(this, p_multiChannelModel, _fileName, HDF5DataIO::Save);
  if (dataSaver.run() == HDF5DataIO::Ok)
  {
    setWindowTitle(("IRoCS Toolbox: " + _fileName).c_str());
    p_multiChannelModel->setModified(false);
  }
  else _fileName = oldFileName;
}

void LabellingMainWidget::saveAs() 
{
  std::string oldFileName = _fileName;
  _fileName = getSaveFileName();
  if (_fileName == "")
  {
    _fileName = oldFileName;
    return;
  }
  HDF5DataIO dataSaver(this, p_multiChannelModel, _fileName, HDF5DataIO::Save);
  if (dataSaver.run() == HDF5DataIO::Ok)
  {
    setWindowTitle(("IRoCS Toolbox: " + _fileName).c_str());
    p_multiChannelModel->setModified(false);
  }
  else _fileName = oldFileName;
}

void LabellingMainWidget::exportToSVG()
{
  SVGDataIO svgWriter(this);
  svgWriter.save();
}

void LabellingMainWidget::exportToAnalyzeAndSTL()
{
  AnalyzeAndSTLDataIO analyzeSTLWriter(this);
  analyzeSTLWriter.save();
}

void LabellingMainWidget::closeProject()
{
  closeProjectInteractive();
}

void LabellingMainWidget::createAnnotationChannel() 
{
  bool ok;
  QString selectedMarkerType = QInputDialog::getItem(
      this, tr("Select marker type"),
      tr("Select the marker type for the new annotation channel"),
      Marker::supportedMarkerTypes(), 0, false, &ok);
  if (!ok) return;
  createAnnotationChannel(Marker::markerTypeFromString(selectedMarkerType));
}

void LabellingMainWidget::createAnnotationChannel(
    Marker::MarkerType mtype)
{
  p_multiChannelModel->addAnnotationChannel(mtype);
}

void LabellingMainWidget::importChannels()
{
  std::string fileName = "";
  if (_initialFileName == "")
  {
    fileName = getOpenFileName();
    if (fileName == "") return;
  }
  else
  {
    fileName = _initialFileName;
    _initialFileName = "";
  }
  HDF5DataIO dataLoader(
      this, p_multiChannelModel, fileName, HDF5DataIO::ImportChannels);
  dataLoader.run();
}

void LabellingMainWidget::importCSVMarkers()
{
  CSVDataIO::importAnnotationChannel(this);
}

void LabellingMainWidget::exportCSVMarkers()
{
  CSVDataIO::exportAnnotationChannel(this);
}

void LabellingMainWidget::showChannelControlWidget(bool show)
{
  p_channelControlDockWidget->setVisible(show);
}


void LabellingMainWidget::showInfoWidget(bool show)
{
  p_infoDockWidget->setVisible(show);
}


void LabellingMainWidget::showOpenGlWidget(bool show)
{
  p_openGlView->setVisible(show);
}


void LabellingMainWidget::showLogWidget(bool show)
{
  p_logWidget->setVisible(show);
}


void LabellingMainWidget::showDebugWidget(bool show)
{
  p_debugWidget->setVisible(show);
}


void LabellingMainWidget::setInterpolation(bool interpolation)
{
  p_orthoView->setInterpolation(interpolation);
  p_orthoView->update();
}


void LabellingMainWidget::snapToVoxelGrid(bool snap)
{
  p_orthoView->setSnapToVoxelGrid(snap);
  p_orthoView->setPositionUm(p_orthoView->positionUm());
}

void LabellingMainWidget::showHelperLines(bool show)
{
  p_orthoView->setShowHelperLines(show);
}

void LabellingMainWidget::runPlugin(QAction* action)
{
  if (_plugins.contains(action))
  {
    QPluginLoader loader(action->data().toString(), this);
    PluginInterface* plugin =
        qobject_cast<PluginInterface*>(loader.instance());
    plugin->setLabellingMainWidget(this);
    _pluginsRunning.push_back(plugin);
    plugin->run();
    _pluginsRunning.remove(plugin);
  }
}

void LabellingMainWidget::setPluginFolder()
{
  if (_pluginsRunning.size() != 0)
  {
    QMessageBox::information(
        this, tr("Plugin folder locked"),
        tr("The plugin folder cannot be changed while plugins are "
           "executed. Please wait until all running plugins finished and "
           "try again."));
    return;
  }
  QSettings settings;
  QString pluginFolder = settings.value(
      "PluginFolder", "").toString();
  QString newPluginFolder = QFileDialog::getExistingDirectory(
      this, tr("Set plugin folder"), pluginFolder,
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
  if (!newPluginFolder.isNull() && newPluginFolder != pluginFolder)
  {
    for (QList<QAction*>::iterator it = _plugins.begin();
         it != _plugins.end(); ++it)
    {
      QPluginLoader loader((*it)->data().toString(), this);
      if (loader.isLoaded() && !loader.unload())
      {
        std::cerr << "Could not unload plugin '" << (*it)->text().toStdString()
                  << "'. The plugin seems to be running." << std::endl;
      }
      else
      {
        p_pluginMenu->removeAction(*it);
        delete *it;
        it = _plugins.erase(it);
        --it;
      }
    }
    settings.setValue("PluginFolder", newPluginFolder);
    searchPlugins();
  }
}

void LabellingMainWidget::runPlugin(
    QString const &name, std::map<std::string,std::string> const &parameters)
{
  QList<QAction*>::const_iterator it = _plugins.begin();
  while (it != _plugins.end() && (*it)->text() != name) ++it;
  if (it == _plugins.end()) return;

  QPluginLoader loader((*it)->data().toString(), this);
  PluginInterface* plugin =
      qobject_cast<PluginInterface*>(loader.instance());
  if (plugin != NULL)
  {
    plugin->setLabellingMainWidget(this);
    plugin->setParameters(parameters);
    _pluginsRunning.push_back(plugin);
    plugin->run();
    _pluginsRunning.remove(plugin);
    loader.unload();
  }
  else
  {
    QMessageBox::critical(
        this, tr("Plugin execution failed"),
        tr("The plugin could not be loaded. Make sure the plugin file exists "
           "and you have read permissions."));
  }
}

iRoCS::ProgressReporterQt4 *LabellingMainWidget::progressReporter()
{
  return p_progressReporter;
}

void LabellingMainWidget::about()
{
  QMessageBox::about(
      this, tr("About iRoCS Toolbox..."),
      tr("This is ") + __labellingVersionString.c_str() + "\n\n" +
      tr("Copyright (C) 2012 Thorsten Schmidt (tschmidt@cs.uni-freiburg.de)\n\n"
         "Address:\n"
         "   Image Analysis Lab\n"
         "   Albert-Ludwigs-Universitaet\n"  
         "   Georges-Koehler-Allee Geb. 52\n"
         "   79110 Freiburg\n"
         "   Germany\n\n") +
      tr("This program is free software: you can redistribute it and/or\n"
         "modify it under the terms of the GNU General Public License\n"
         "Version 3 as published by the Free Software Foundation.\n\n"
         "This program is distributed in the hope that it will be useful,\n"
         "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
         "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the\n"
         "GNU General Public License for more details.\n\n"
         "You should have received a copy of the GNU General Public License\n"
         "along with this program.  If not, see\n"
         "<http://www.gnu.org/licenses/>."));
}

void LabellingMainWidget::setUserInteractionEnabled(bool enable)
{
  p_fileMenu->setEnabled(enable);
  p_channelMenu->setEnabled(enable);
  p_pluginMenu->setEnabled(enable);
  p_orthoView->setEnabled(enable);
  p_channelControlDockWidget->setEnabled(enable);
}

void LabellingMainWidget::showError(const QString& message)
{
  QMessageBox::critical(this, "IRoCS Toolbox", message);
}

size_t LabellingMainWidget::memoryLimit() const
{
  return _memoryLimit;
}

void LabellingMainWidget::searchPlugins()
{
  QSettings settings;
  QString pluginFolder = settings.value("PluginFolder", "").toString();
  QDir pluginsDir;
  if (pluginFolder == "")
  {
    pluginsDir = QDir(qApp->applicationDirPath());
    pluginsDir.cdUp();
    pluginsDir.cd("share");
    pluginsDir.cd("irocs-toolbox");
    pluginsDir.cd("plugins");
    pluginFolder = pluginsDir.absolutePath();
    settings.setValue("PluginFolder", pluginFolder);
  }
  else pluginsDir = QDir(pluginFolder);
  std::cout << "Searching plugins in "
            << pluginFolder.toStdString() << std::endl;

  foreach (QString fileName, pluginsDir.entryList(QDir::Files))
  {
#ifdef _WIN32
    if (!fileName.endsWith(".dll", Qt::CaseInsensitive)) continue;
#else
    if (!fileName.endsWith(".so", Qt::CaseInsensitive)) continue;
#endif
    QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
    QObject* plugin = loader.instance();
    if (plugin)
    {
      PluginInterface* iPlugin = qobject_cast<PluginInterface*>(plugin);
      if (iPlugin)
      {
        std::cout << "Registering plugin: " << iPlugin->name().toStdString()
                  << std::endl;
        QAction *action = new QAction(iPlugin->name(), NULL);
        action->setData(pluginsDir.absoluteFilePath(fileName));
        QList<QAction*>::iterator it = _plugins.begin();
        while (it != _plugins.end() &&
               (*it)->text().compare(iPlugin->name()) < 0) ++it;
        _plugins.insert(it, action);
      }
      else
      {
        std::cerr << fileName.toStdString()
                  << " is no valid iRoCS Toolbox plugin. It does not "
                  << "implement the PluginInterface class or does not "
                  << "inherit from QObject" << std::endl;
      }
      loader.unload();
    }
    else
    {
      std::cerr << fileName.toStdString() << " is no valid QT plugin: "
                << loader.errorString().toStdString() << std::endl;
    }
  }
  for (QList<QAction*>::iterator it = _plugins.begin();
       it != _plugins.end(); ++it) p_pluginMenu->addAction(*it);
}

std::string LabellingMainWidget::getOpenFileName() 
{
  QSettings settings;
  QString startPath = settings.value(
      "FileDialog/startpath", getenv("HOME")).toString();
  QString selectedFilter;
  QString fileName = QFileDialog::getOpenFileName(
      this, tr("Load Data Channel"), startPath,
      tr("HDF5 (*.h5);;Imaris (*.ims)"), &selectedFilter);
  if (fileName == QString::Null()) return "";
  settings.setValue("FileDialog/startpath", fileName);
  return fileName.toStdString();
}

std::string LabellingMainWidget::getSaveFileName() 
{
  QSettings settings;
  QString startPath = settings.value(
      "FileDialog/startpath", getenv("HOME")).toString();
  QString fileName = QFileDialog::getSaveFileName(
      this, tr("Save iRoCS project"), startPath, tr("HDF5 (*.h5)"));
  if (fileName == QString::Null()) return "";
  settings.setValue("FileDialog/startpath", fileName);
  return fileName.toStdString();
}

void LabellingMainWidget::closeEvent(QCloseEvent *e) 
{
  if (!closeProjectInteractive())
  {
    e->ignore();
    return;
  }
  e->accept();
  for (std::list<PluginInterface*>::iterator it = _pluginsRunning.begin();
       it != _pluginsRunning.end(); ++it) (*it)->abort();
  QApplication::quit();
}

void LabellingMainWidget::resizeEvent(QResizeEvent*)
{
  p_progressBar->setFixedWidth(width() / 4);
}
