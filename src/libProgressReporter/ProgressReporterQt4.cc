#include "ProgressReporterQt4.hh"

#include <iostream>

namespace iRoCS
{

  ProgressReporterQt4::ProgressReporterQt4(
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
  
  ProgressReporterQt4::~ProgressReporterQt4()
  {}
  
  void ProgressReporterQt4::setProgressMin(int progressMin)
  {
    emit progressMinChanged(progressMin);
  }

  int ProgressReporterQt4::progressMin() const
  {
    if (p_progressBar != NULL) return p_progressBar->minimum();
    else return -1;
  }

  void ProgressReporterQt4::setProgressMax(int progressMax)
  {
    emit progressMaxChanged(progressMax);
  }

  int ProgressReporterQt4::progressMax() const
  {
    if (p_progressBar != NULL) return p_progressBar->maximum();
    else return -1;
  }

  void ProgressReporterQt4::setHeaderMessage(std::string const &message)
  {
    emit headerMessageChanged(message.c_str());
  }

  std::string ProgressReporterQt4::headerMessage() const
  {
    if (p_headerLabel != NULL)
        return p_headerLabel->text().toStdString();
    else return std::string();
  }

  void ProgressReporterQt4::setProgressMessage(std::string const &message)
  {
    emit progressMessageChanged(message.c_str());
  }

  std::string ProgressReporterQt4::progressMessage() const
  {
    if (p_messageLabel != NULL)
        return p_messageLabel->text().toStdString();
    else return std::string();
  }
  
  void ProgressReporterQt4::setProgress(int progress)
  {
    emit progressChanged(progress);
  }
  
  int ProgressReporterQt4::progress() const
  {
    if (p_progressBar != NULL) return p_progressBar->value();
    else return -1;
  }

  void ProgressReporterQt4::setAborted(bool abort)
  {
    _aborted = abort;
  }

  void ProgressReporterQt4::abort()
  {
    _aborted = true;
  }

  void ProgressReporterQt4::abortWithError(std::string const &msg)
  {
    _aborted = true;
    emit error(msg.c_str());
  }

  bool ProgressReporterQt4::isAborted() const
  {
    return _aborted;
  }

  bool ProgressReporterQt4::updateProgress(int progress)
  {
    setProgress(progress);
    qApp->processEvents();
    return !_aborted;
  }

  bool ProgressReporterQt4::updateProgressMessage(std::string const &message)
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

  void ProgressReporterQt4::setVisible(bool visible)
  {
    if (p_progressBar != NULL) p_progressBar->setVisible(visible);
    if (p_headerLabel != NULL) p_headerLabel->setVisible(visible);
    if (p_messageLabel != NULL) p_messageLabel->setVisible(visible);
    if (p_cancelButton != NULL) p_cancelButton->setVisible(visible);
  }

  void ProgressReporterQt4::reset()
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

  void ProgressReporterQt4::cancel()
  {
    abort();
  }

}
