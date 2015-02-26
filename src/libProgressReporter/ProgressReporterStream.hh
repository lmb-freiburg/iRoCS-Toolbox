#ifndef IROCSPROGRESSREPORTERSTREAM_HH
#define IROCSPROGRESSREPORTERSTREAM_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "ProgressReporter.hh"
#include <iostream>

namespace iRoCS
{
  
  class ProgressReporterStream : public ProgressReporter
  {

  public:
  
    ProgressReporterStream(
        std::ostream& os = std::cout,
        int progress = 0, int progressMin = 0, int progressMax = 100,
        std::string const &headerMessage = std::string(),
        std::string const &progressMessage = std::string());

    ~ProgressReporterStream();

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

  private:
  
    std::ostream& _os;

    int _progress, _progressMin, _progressMax;
    std::string _headerMessage, _progressMessage;
    bool _aborted;

  };

}

#endif
