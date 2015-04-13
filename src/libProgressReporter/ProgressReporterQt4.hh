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

#ifndef ATBPROGRESSREPORTERQT4_HH
#define ATBPROGRESSREPORTERQT4_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "ProgressReporter.hh"

#include <QtGui/QApplication>
#include <QtGui/QProgressBar>
#include <QtGui/QLabel>
#include <QtGui/QAbstractButton>

namespace iRoCS
{
  
  class ProgressReporterQt4 :
        public QObject, public ProgressReporter
  {

    Q_OBJECT

    public:
  
    ProgressReporterQt4(
        QProgressBar *progressBar = NULL, QLabel *headerLabel = NULL,
        QLabel *messageLabel = NULL, QAbstractButton *cancelButton = NULL,
        std::ostream *stream = NULL);
  
    ~ProgressReporterQt4();
    
    void setProgressMin(int progressMin);

    int progressMin() const;

    void setProgressMax(int progressMax);

    int progressMax() const;

    void setHeaderMessage(std::string const &message);

    std::string headerMessage() const;

    void setProgressMessage(std::string const &message);

    std::string progressMessage() const;
  
    void setProgress(int progress);
  
    int progress() const;

    void setAborted(bool abort);

    void abort();

    void abortWithError(std::string const &msg);

    bool isAborted() const;

    bool updateProgress(int progress);

    bool updateProgressMessage(std::string const &message);

  signals:

    void progressMinChanged(int);
    void progressMaxChanged(int);
    void progressChanged(int);
    void headerMessageChanged(QString);
    void progressMessageChanged(QString);
    void error(QString const &);

  public slots:

    void setVisible(bool visible);
    void reset();

  private slots:
  
    void cancel();
  
  private:
  
    QProgressBar *p_progressBar;
    QLabel *p_headerLabel, *p_messageLabel;
    QAbstractButton *p_cancelButton;

    bool _aborted;
    std::ostream *p_stream;
  
  };

}

#endif
