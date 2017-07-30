/**************************************************************************
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

#include "iRoCSMainWindow.hh"

#include <QtCore/QCoreApplication>
#include <QtCore/QResource>
#include <QtCore/QSettings>

#include <QtGui/QAction>
#include <QtGui/QMenuBar>
#include <QtGui/QMenu>
#include <QtGui/QToolBar>
#include <QtGui/QToolButton>
#include <QtGui/QFileDialog>
#include <QtGui/QDockWidget>
#include <QtGui/QStatusBar>
#include <QtGui/QMessageBox>
#include <QtGui/QCloseEvent>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QSplitter>

#include <libMultiChannelModel/MultichannelModel.hh>
#include "OrthoviewWidget.hh"
#include "MultichannelModelTreeWidget.hh"
#include "MultichannelModelControlWidgetStack.hh"
#include "ChannelTreeWidgetItem.hh"
#include "HDF5SelectionDialog.hh"
#include "Double3ControlElement.hh"
#include "ChannelControlWidget.hh"

#ifndef PACKAGE_STRING
#define PACKAGE_STRING "iRoCS Toolbox 2.0.0"
#endif
#ifndef PACAKGE_URL
#define PACKAGE_URL "http://lmb.informatik.uni-freiburg.de/lmbsoft/iRoCS"
#endif
#ifndef PACAKGE_BUGREPORT
#define PACKAGE_BUGREPORT "falk@cs.uni-freiburg.de"
#endif

#define DEBUG

namespace iRoCS {

#ifdef DEBUG
  std::string const iRoCSMainWindow::__iRoCSVersionString =
    std::string(PACKAGE_STRING " debug last build: ") +
    __DATE__ + " " + __TIME__;
#else
  std::string const iRoCSMainWindow::__iRoCSVersionString =
    std::string(PACKAGE_STRING " last build: ") +
    __DATE__ + " " + __TIME__;
#endif

  iRoCSMainWindow::iRoCSMainWindow(
    size_t memoryLimit, QString const &fileName, bool forceImport,
    QWidget *parent, Qt::WindowFlags f)
    : QMainWindow(parent, f), _memoryLimit(memoryLimit),
    _fileName(fileName), p_model(NULL), p_orthoview(NULL),
    p_modelTree(NULL), p_logBuffer(NULL),
    p_origCoutBuffer(NULL), p_debugBuffer(NULL), p_origCerrBuffer(NULL),
    p_deleteChannelsAction(NULL), p_progressReporter(NULL) {
    QCoreApplication::setOrganizationName("LMB");
    QCoreApplication::setOrganizationDomain("lmb.informatik.uni-freiburg.de");
    QCoreApplication::setApplicationName("iRoCS Toolbox");
    QString resourceDir =
      QCoreApplication::applicationDirPath() + "/../share/irocs-toolbox";
    setWindowIcon(QIcon(":/iRoCSToolboxIcon.svg"));
    QSettings settings;
    resize(settings.value("MainWindow/size", QSize(800, 600)).toSize());
    setObjectName("iRoCS-Toolbox|iRoCSMainWindow");

    setWindowTitle("iRoCS Toolbox");

    QDockWidget *logWidget = new QDockWidget("Log", this);
    QTextEdit *logTextEdit = new QTextEdit;
    logTextEdit->setReadOnly(true);
    p_logBuffer = new StreamBufTextEditQt4(logTextEdit);
#ifndef DEBUG
    p_origCoutBuffer = std::cout.rdbuf(p_logBuffer);
#endif
    logWidget->setWidget(logTextEdit);
    addDockWidget(Qt::BottomDockWidgetArea, logWidget);
    logWidget->setVisible(false);

    QDockWidget *debugWidget = new QDockWidget("Warnings", this);
    QTextEdit *debugTextEdit = new QTextEdit;
    debugTextEdit->setReadOnly(true);
    p_debugBuffer = new StreamBufTextEditQt4(debugTextEdit);
#ifndef DEBUG
    p_origCerrBuffer = std::cerr.rdbuf(p_debugBuffer);
#endif
    debugWidget->setWidget(debugTextEdit);
    addDockWidget(Qt::BottomDockWidgetArea, debugWidget);
    debugWidget->setVisible(false);

    p_model = new MultichannelModel();

    p_orthoview = new OrthoviewWidget(*p_model);

    p_modelTree = new MultichannelModelTreeWidget(*p_model);
    connect(p_modelTree,
      SIGNAL(itemSelectionChanged()), SLOT(updateModelControlStack()));

    p_modelControlStack = new MultichannelModelControlWidgetStack(*p_model);
    setCentralWidget(p_orthoview);

    QDockWidget *modelControlDockWidget = new QDockWidget(
      tr("Channel Control"), this);
    QSplitter *modelControlSplitter = new QSplitter(Qt::Vertical);
    modelControlSplitter->addWidget(p_modelTree);
    modelControlSplitter->addWidget(p_modelControlStack);
    modelControlDockWidget->setWidget(modelControlSplitter);
    addDockWidget(Qt::RightDockWidgetArea, modelControlDockWidget);

    p_infoWidget = new QWidget;
    QVBoxLayout *infoLayout = new QVBoxLayout(p_infoWidget);
    p_infoWidget->setSizePolicy(
      QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    Double3ControlElement *mousePositionControlWidget =
      new Double3ControlElement(tr("Position [mu]"));
    mousePositionControlWidget->setActive(false);
    connect(p_orthoview,
      SIGNAL(mousePositionChanged(blitz::TinyVector<double, 3> const &)),
      mousePositionControlWidget,
      SLOT(setValue(blitz::TinyVector<double, 3> const &)));
    infoLayout->addWidget(mousePositionControlWidget);
    p_valueLineEdit = new QLineEdit;
    p_valueLineEdit->setReadOnly(true);
    infoLayout->addWidget(p_valueLineEdit);
    connect(p_orthoview,
      SIGNAL(mousePositionChanged(blitz::TinyVector<double, 3> const &)),
      SLOT(updateInfoWidget(blitz::TinyVector<double, 3> const &)));
    QDockWidget *infoDockWidget = new QDockWidget(
      tr("Information"), this);
    infoDockWidget->setWidget(p_infoWidget);
    addDockWidget(Qt::RightDockWidgetArea, infoDockWidget);

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(
      tr("&Open..."), this, SLOT(open()),
      QKeySequence(tr("Ctrl+O", "File|Open")));
    QAction* fileSaveAction =
      fileMenu->addAction(
        tr("&Save"), this, SLOT(save()),
        QKeySequence(tr("Ctrl+S", "File|Save")));
    fileSaveAction->setEnabled(false);
    connect(p_modelTree, SIGNAL(modifiedFlagChanged(bool)),
      fileSaveAction, SLOT(setEnabled(bool)));
    QAction* fileSaveAsAction =
      fileMenu->addAction(tr("Save as..."), this, SLOT(saveAs()));
    fileSaveAsAction->setEnabled(false);
    connect(p_modelTree, SIGNAL(isEmptyChanged(bool)), fileSaveAsAction,
      SLOT(setDisabled(bool)));
    QAction* fileCloseAction =
      fileMenu->addAction(tr("Close"), this, SLOT(closeProject()));
    fileCloseAction->setEnabled(false);
    connect(p_modelTree, SIGNAL(isEmptyChanged(bool)), fileCloseAction,
      SLOT(setDisabled(bool)));
    fileMenu->addSeparator();
    fileMenu->addAction(
      tr("E&xit"), this, SLOT(close()),
      QKeySequence(tr("Ctrl+X", "File|Exit")));

    QMenu *channelMenu = menuBar()->addMenu(tr("&Channel"));
    QAction *importChannelsAction = new QAction(
      QIcon::fromTheme("document-open"), "Import channels...", this);
    importChannelsAction->setToolTip(tr("Import channels..."));
    connect(importChannelsAction, SIGNAL(triggered()), SLOT(importChannels()));
    channelMenu->addAction(importChannelsAction);

    p_deleteChannelsAction = new QAction(
      QIcon::fromTheme("edit-delete"), "Delete channels", this);
    p_deleteChannelsAction->setToolTip(tr("Delete selected channels"));
    p_deleteChannelsAction->setEnabled(false);
    connect(p_modelTree, SIGNAL(itemSelectionChanged()),
      SLOT(setDeleteChannelsActionState()));
    connect(p_deleteChannelsAction, SIGNAL(triggered()),
      p_modelTree, SLOT(deleteSelectedChannels()));
    channelMenu->addAction(p_deleteChannelsAction);

    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(modelControlDockWidget->toggleViewAction());
    viewMenu->addAction(infoDockWidget->toggleViewAction());
    viewMenu->addAction(logWidget->toggleViewAction());
    viewMenu->addAction(debugWidget->toggleViewAction());

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr("About"), this, SLOT(about()));
    helpMenu->addAction(tr("About Qt"), qApp, SLOT(aboutQt()));

    QLabel *statusText = new QLabel;
    statusBar()->addWidget(statusText);
    QProgressBar *progressBar = new QProgressBar;
    progressBar->setFixedWidth(width() / 4);
    progressBar->setVisible(false);
    statusBar()->addPermanentWidget(progressBar);
    QToolButton *cancelButton = new QToolButton;
    cancelButton->setIcon(
      QIcon::fromTheme("process-stop", QIcon(":/process-stop.png")));
    cancelButton->setText(tr("cancel"));
    cancelButton->setToolTip(tr("Cancel current operation"));
    cancelButton->setVisible(false);
    statusBar()->addPermanentWidget(cancelButton);
    p_progressReporter = new iRoCS::ProgressReporterQt4(
      progressBar, NULL, statusText, cancelButton, &std::cout);
    connect(p_progressReporter, SIGNAL(error(QString const &)),
      SLOT(showError(QString const &)));

    show();

    // Output status message
    std::vector<std::string> units;
    units.push_back("B");
    units.push_back("kB");
    units.push_back("MB");
    units.push_back("GB");
    units.push_back("TB");
    int currentUnit = 0;
    double m = memoryLimit;
    while (m >= 1024.0 && currentUnit < 4) {
      currentUnit++;
      m /= 1024.0;
    }
    std::cout << PACKAGE_STRING << " initialized. Memory limit: "
      << std::fixed << std::setprecision(1) << m << " "
      << units[currentUnit] << std::endl;

    if (fileName != "") {
      bool isLabellingProject = false;
      try {
        {
          BlitzH5File inFile(fileName.toStdString());
          isLabellingProject = inFile.existsGroup(".labelling_qt4");
        }
        if (!forceImport && isLabellingProject) open(fileName);
        else importChannels(fileName);
      }
      catch (BlitzH5Error &e) {
        showError(tr("Could not open '%1' for reading: %2").arg(
          fileName).arg(e.what()));
      }
    }
  }

  iRoCSMainWindow::~iRoCSMainWindow() {
    QSettings settings;
    settings.setValue("MainWindow/size", size());

    delete p_model;
    delete p_progressReporter;

#ifndef DEBUG
    std::cout.rdbuf(p_origCoutBuffer);
#endif
    delete p_logBuffer;
#ifndef DEBUG
    std::cerr.rdbuf(p_origCerrBuffer);
#endif
    delete p_debugBuffer;
  }

  iRoCS::ProgressReporterQt4 *iRoCSMainWindow::progressReporter() {
    return p_progressReporter;
  }

  std::string iRoCSMainWindow::fileName() const {
    return _fileName.toStdString();
  }

  size_t iRoCSMainWindow::memoryLimit() const {
    return _memoryLimit;
  }

  bool iRoCSMainWindow::closeProjectInteractive() {
    if (p_model->modified()) {
      QMessageBox::StandardButton res = QMessageBox::question(
        this, tr("Save model"),
        tr("The current model has unsaved data, do you want to save it?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
        QMessageBox::Save);
      switch (res) {
        case QMessageBox::Save:
          save();
          if (p_model->modified()) return false;
          break;
        case QMessageBox::Discard:
          break;
        default:
          return false;
          break;
      }
    }
    p_model->clear();
    _fileName = "";
    setWindowTitle("IRoCS Toolbox");
    return true;
  }

  void iRoCSMainWindow::open(QString const &fileName) {
    if (!closeProjectInteractive()) return;
    _fileName = (fileName == "") ? getOpenFileName() : fileName;
    if (_fileName == "") return;
    p_progressReporter->reset();
    p_progressReporter->setVisible(true);
    try {
      BlitzH5File inFile(_fileName.toStdString());
      p_model->load(inFile, p_progressReporter);
      setWindowTitle(tr("IRoCS Toolbox: %1").arg(_fileName));
      p_progressReporter->setVisible(false);
    }
    catch (BlitzH5Error &e) {
      p_progressReporter->setVisible(false);
      p_progressReporter->abortWithError(
        tr("The file you selected is no valid labelling project! Use "
          "'Channel -> Import Channels...' to open plain hdf5 datasets "
          "and annotations.\n\nHdf5 Error: %1").arg(e.what()).toStdString());
      _fileName = "";
    }
  }

  void iRoCSMainWindow::save() {
    QString oldFileName = _fileName;
    if (_fileName == "" && (_fileName = getSaveFileName()) == "") return;
    p_progressReporter->reset();
    p_progressReporter->setVisible(true);
    try {
      BlitzH5File outFile(_fileName.toStdString(), BlitzH5File::WriteOrNew);
      p_model->save(outFile, p_progressReporter);
      setWindowTitle(tr("IRoCS Toolbox: %1").arg(_fileName));
      p_progressReporter->setVisible(false);
    }
    catch (BlitzH5Error &e) {
      p_progressReporter->setVisible(false);
      p_progressReporter->abortWithError(
        tr("Could not save iRoCS Toolbox project.\n\nHdf5 Error: %1").arg(
          e.what()).toStdString());
      _fileName = oldFileName;
    }
  }

  void iRoCSMainWindow::saveAs() {
    QString oldFileName = _fileName;
    if ((_fileName = getSaveFileName()) == "") {
      _fileName = oldFileName;
      return;
    }
    p_progressReporter->reset();
    p_progressReporter->setVisible(true);
    try {
      BlitzH5File outFile(_fileName.toStdString(), BlitzH5File::WriteOrNew);
      p_model->save(outFile, p_progressReporter);
      setWindowTitle(tr("IRoCS Toolbox: %1").arg(_fileName));
      p_progressReporter->setVisible(false);
    }
    catch (BlitzH5Error &e) {
      p_progressReporter->setVisible(false);
      p_progressReporter->abortWithError(
        tr("Could not save iRoCS Toolbox project.\n\nHdf5 Error: %1").arg(
          e.what()).toStdString());
      _fileName = oldFileName;
    }
  }

  void iRoCSMainWindow::closeProject() {
    closeProjectInteractive();
  }

  void iRoCSMainWindow::importChannels(QString const &fileName) {
    if (fileName == "") {
      _fileName = getOpenFileName();
      if (_fileName == "") return;
    }
    else _fileName = fileName;

    p_progressReporter->reset();
    p_progressReporter->setVisible(true);
    std::vector<std::string> selectedItems =
      HDF5SelectionDialog::selectObjects(
        _fileName, p_model, this, Channel::Any, p_progressReporter);
    p_progressReporter->setVisible(false);
    if (selectedItems.size() == 0) return;

    p_progressReporter->reset();
    p_progressReporter->setVisible(true);
    for (size_t i = 0; i < selectedItems.size(); ++i) {
      p_progressReporter->setTaskProgressMin(
        static_cast<int>(100 * static_cast<double>(i) /
          static_cast<double>(selectedItems.size())));
      p_progressReporter->setTaskProgressMax(
        static_cast<int>(100 * static_cast<double>(i + 1) /
          static_cast<double>(selectedItems.size())));
      p_progressReporter->updateProgressMessage(
        "Importing '" + selectedItems[i] + "'");
      try {
        BlitzH5File inFile(_fileName.toStdString());
        Channel *channel = Channel::createFromFile(
          inFile, selectedItems[i], p_progressReporter);
        if (p_progressReporter->isAborted()) return;
        p_model->addChannel(channel);
      }
      catch (BlitzH5Error &e) {
        showError(
          tr("Could not import selected channels.\n\nHdf5 Error: %1").arg(
            e.what()));
      }
    }
    p_progressReporter->setVisible(false);
    setWindowTitle("IRoCS Toolbox: " + _fileName);
  }

  void iRoCSMainWindow::setDeleteChannelsActionState() {
    p_deleteChannelsAction->setEnabled(
      p_modelTree->selectedItems().size() != 0);
  }

  void iRoCSMainWindow::updateInfoWidget(
    blitz::TinyVector<double, 3> const &positionUm) {
    QString valueString;
    for (MultichannelModel::const_iterator it = p_model->begin();
      it != p_model->end(); ++it)
      valueString += (" " + (*it)->stringValueAt(positionUm)).c_str();
    p_valueLineEdit->setText(valueString);
  }

  void iRoCSMainWindow::updateModelControlStack() {
    if (p_modelTree->selectedItems().size() != 1 ||
      p_modelTree->selectedItems()[0]->type() < QTreeWidgetItem::UserType) {
      p_modelControlStack->setCurrentIndex(-1);
      p_modelControlStack->setVisible(false);
      return;
    }
    p_modelControlStack->setCurrentWidget(
      ChannelControlWidget::instance(
        static_cast<ChannelTreeWidgetItem*>(
          p_modelTree->selectedItems()[0])->channel(),
        p_modelControlStack));
    p_modelControlStack->setVisible(true);
  }

  void iRoCSMainWindow::about() {
    QMessageBox::about(
      this, tr("About iRoCS Toolbox..."),
      tr("This is %1\n\n"
        "URL: %2\n\n"
        "Copyright (C) 2012-2015 Thorsten Falk (%3)\n\n"
        "Address:\n"
        "   Image Analysis Lab\n"
        "   Albert-Ludwigs-Universitaet\n"
        "   Georges-Koehler-Allee Geb. 52\n"
        "   79110 Freiburg\n"
        "   Germany\n\n"
        "This program is free software: you can redistribute it and/or\n"
        "modify it under the terms of the GNU General Public License\n"
        "Version 3 as published by the Free Software Foundation.\n\n"
        "This program is distributed in the hope that it will be useful,\n"
        "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
        "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the\n"
        "GNU General Public License for more details.\n\n"
        "You should have received a copy of the GNU General Public License\n"
        "along with this program.  If not, see\n"
        "<http://www.gnu.org/licenses/>.").arg(
          __iRoCSVersionString.c_str()).arg(
            PACKAGE_URL).arg(PACKAGE_BUGREPORT));
  }

  void iRoCSMainWindow::showError(const QString& message) {
    QMessageBox::critical(this, "IRoCS Toolbox", message);
  }

  QString iRoCSMainWindow::getOpenFileName() {
    QSettings settings;
    QString startPath = settings.value(
      "FileDialog/startpath", getenv("HOME")).toString();
    QString selectedFilter;
    QString fileName = QFileDialog::getOpenFileName(
      this, tr("Load Data Channel"), startPath,
      tr("HDF5 (*.h5);;Imaris (*.ims)"), &selectedFilter);
    if (fileName == QString::Null()) return "";
    settings.setValue("FileDialog/startpath", fileName);
    return fileName;
  }

  QString iRoCSMainWindow::getSaveFileName() {
    QSettings settings;
    QString startPath = settings.value(
      "FileDialog/startpath", getenv("HOME")).toString();
    QString fileName = QFileDialog::getSaveFileName(
      this, tr("Save iRoCS project"), startPath, tr("HDF5 (*.h5)"));
    if (fileName == QString::Null()) return "";
    settings.setValue("FileDialog/startpath", fileName);
    return fileName;
  }

  void iRoCSMainWindow::closeEvent(QCloseEvent *e) {
    if (!closeProjectInteractive()) {
      e->ignore();
      return;
    }
    e->accept();
    QApplication::quit();
  }

  void iRoCSMainWindow::resizeEvent(QResizeEvent *) {
    if (p_progressReporter != NULL)
      p_progressReporter->progressBar()->setFixedWidth(width() / 4);
  }

}
