#ifndef ATBPROGRESSREPORTERQT4_HH
#define ATBPROGRESSREPORTERQT4_HH

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
