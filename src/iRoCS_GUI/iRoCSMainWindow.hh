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

#ifndef IROCS_IROCSMAINWINDOW_HH
#define IROCS_IROCSMAINWINDOW_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QMainWindow>

#include "StreamBufTextEditQt4.hh"
#include <libProgressReporter_qt4/ProgressReporterQt4.hh>

#include <blitz/array.h>

#if defined(DEBUG) && defined(_WIN32)
#include <fstream>
#endif

class QAction;
class QLineEdit;
class QTreeWidgetItem;

namespace iRoCS {

  class MultichannelModel;
  class OrthoviewWidget;
  class MultichannelModelTreeWidget;
  class MultichannelModelControlWidgetStack;

  class iRoCSMainWindow : public QMainWindow {

    Q_OBJECT

  public:

    static std::string const __iRoCSVersionString;

    iRoCSMainWindow(
      size_t memoryLimit = 0, QString const &fileName = "",
      bool forceImport = false, QWidget *parent = NULL,
      Qt::WindowFlags f = 0);
    ~iRoCSMainWindow();

    iRoCS::ProgressReporterQt4 *progressReporter();

    std::string fileName() const;
    size_t memoryLimit() const;

    bool closeProjectInteractive();

    private slots:

    void open(QString const &fileName = "");
    void save();
    void saveAs();
    void closeProject();
    void importChannels(QString const &fileName = "");
    void setDeleteChannelsActionState();
    void updateInfoWidget(blitz::TinyVector<double, 3> const &positionUm);
    void updateModelControlStack();

    void about();

    void showError(QString const &message);

  private:

    QString getOpenFileName();
    QString getSaveFileName();

    void closeEvent(QCloseEvent *e);
    void resizeEvent(QResizeEvent *);

    size_t _memoryLimit;
    QString _fileName;

    MultichannelModel *p_model;
    OrthoviewWidget *p_orthoview;
    MultichannelModelTreeWidget *p_modelTree;
    MultichannelModelControlWidgetStack *p_modelControlStack;

    QLineEdit *p_valueLineEdit;
    QWidget *p_infoWidget;

    StreamBufTextEditQt4 *p_logBuffer;
    std::streambuf *p_origCoutBuffer;

    StreamBufTextEditQt4* p_debugBuffer;
    std::streambuf *p_origCerrBuffer;

    QAction *p_deleteChannelsAction;

    iRoCS::ProgressReporterQt4 *p_progressReporter;

#if defined(DEBUG) && defined(_WIN32)
    std::ofstream _coutFile, _cerrFile;
#endif

  };

}

#endif
