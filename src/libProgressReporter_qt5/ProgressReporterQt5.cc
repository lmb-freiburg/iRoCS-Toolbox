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

#include "ProgressReporterQt5.hh"

#include <iostream>

namespace iRoCS
{

  ProgressReporterQt5::ProgressReporterQt5(
      QProgressBar *progressBar, QLabel *headerLabel,
      QLabel *messageLabel, QAbstractButton *cancelButton,
      std::ostream *stream)
          : QObject(), ProgressReporter(), p_progressBar(progressBar),
            p_headerLabel(headerLabel), p_messageLabel(messageLabel),
            p_cancelButton(cancelButton), _aborted(false), p_stream(stream)
  {
    if (cancelButton != NULL)
        connect(cancelButton, SIGNAL(clicked()), SLOT(cancel()));
    if (p_progressBar != NULL)
    {
      connect(this, SIGNAL(progressMinChanged(int)),
              p_progressBar, SLOT(setMinimum(int)));
      connect(this, SIGNAL(progressMaxChanged(int)),
              p_progressBar, SLOT(setMaximum(int)));
      connect(this, SIGNAL(progressChanged(int)),
              p_progressBar, SLOT(setValue(int)));
    }
    if (p_headerLabel != NULL)
    {
      connect(this, SIGNAL(headerMessageChanged(QString)),
              p_headerLabel, SLOT(setText(QString)));
    }          
    if (p_messageLabel != NULL)
    {
      connect(this, SIGNAL(progressMessageChanged(QString)),
              p_messageLabel, SLOT(setText(QString)));
    }
  }
  
  ProgressReporterQt5::~ProgressReporterQt5()
  {}
  
  void ProgressReporterQt5::setProgressMin(int progressMin)
  {
    emit progressMinChanged(progressMin);
  }

  int ProgressReporterQt5::progressMin() const
  {
    if (p_progressBar != NULL) return p_progressBar->minimum();
    else return -1;
  }

  void ProgressReporterQt5::setProgressMax(int progressMax)
  {
    emit progressMaxChanged(progressMax);
  }

  int ProgressReporterQt5::progressMax() const
  {
    if (p_progressBar != NULL) return p_progressBar->maximum();
    else return -1;
  }

  void ProgressReporterQt5::setHeaderMessage(std::string const &message)
  {
    emit headerMessageChanged(message.c_str());
  }

  std::string ProgressReporterQt5::headerMessage() const
  {
    if (p_headerLabel != NULL)
        return p_headerLabel->text().toStdString();
    else return std::string();
  }

  void ProgressReporterQt5::setProgressMessage(std::string const &message)
  {
    emit progressMessageChanged(message.c_str());
  }

  std::string ProgressReporterQt5::progressMessage() const
  {
    if (p_messageLabel != NULL)
        return p_messageLabel->text().toStdString();
    else return std::string();
  }
  
  void ProgressReporterQt5::setProgress(int progress)
  {
    emit progressChanged(progress);
  }
  
  int ProgressReporterQt5::progress() const
  {
    if (p_progressBar != NULL) return p_progressBar->value();
    else return -1;
  }

  void ProgressReporterQt5::setAborted(bool abort)
  {
    _aborted = abort;
  }

  void ProgressReporterQt5::abort()
  {
    _aborted = true;
  }

  void ProgressReporterQt5::abortWithError(std::string const &msg)
  {
    _aborted = true;
    emit error(msg.c_str());
  }

  bool ProgressReporterQt5::isAborted() const
  {
    return _aborted;
  }

  bool ProgressReporterQt5::updateProgress(int progress)
  {
    setProgress(progress);
    qApp->processEvents();
    return !_aborted;
  }

  bool ProgressReporterQt5::updateProgressMessage(std::string const &message)
  {
    setProgressMessage(message);

    if (p_stream != NULL)
    {
#ifdef _OPENMP
#pragma omp critical
#endif
      *p_stream << message << std::endl;
    }
    qApp->processEvents();
    return !_aborted;
  }

  void ProgressReporterQt5::setVisible(bool visible)
  {
    if (p_progressBar != NULL) p_progressBar->setVisible(visible);
    if (p_headerLabel != NULL) p_headerLabel->setVisible(visible);
    if (p_messageLabel != NULL) p_messageLabel->setVisible(visible);
    if (p_cancelButton != NULL) p_cancelButton->setVisible(visible);
  }

  void ProgressReporterQt5::reset()
  {
    setProgressMin(0);
    setProgressMax(100);
    setTaskProgressMin(0);
    setTaskProgressMax(100);
    setHeaderMessage("");
    setProgressMessage("");
    setProgress(0);
    setAborted(false);
  }

  void ProgressReporterQt5::cancel()
  {
    abort();
  }

  QProgressBar *ProgressReporterQt5::progressBar() const
  {
    return p_progressBar;
  }

}
