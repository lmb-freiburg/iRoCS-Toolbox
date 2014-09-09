#ifndef IROCSPROGRESSREPORTER_HH
#define IROCSPROGRESSREPORTER_HH

#include <string>

namespace iRoCS
{
  
  class ProgressReporter
  {

  public:

    ProgressReporter()
            : _taskProgressMin(0), _taskProgressMax(0)
          {}

    virtual ~ProgressReporter()
          {}

    virtual void setProgressMin(int progressMin) = 0;

    virtual int progressMin() const = 0;

    virtual void setProgressMax(int progressMax) = 0;

    virtual int progressMax() const = 0;

    void setTaskProgressRange(int progressMin, int progressMax);

    void setTaskProgressMin(int progressMin);

    int taskProgressMin() const;

    void setTaskProgressMax(int progressMax);

    int taskProgressMax() const;

    virtual void setHeaderMessage(std::string const &message) = 0;

    virtual std::string headerMessage() const = 0;

    virtual void setProgressMessage(std::string const &message) = 0;

    virtual std::string progressMessage() const = 0;
  
    virtual void setProgress(int progress) = 0;
  
    virtual int progress() const = 0;

    virtual void setAborted(bool abort) = 0;

    virtual void abort() = 0;

    virtual void abortWithError(std::string const &msg) = 0;

    virtual bool isAborted() const = 0;

    virtual bool updateProgress(int progress) = 0;

    virtual bool updateProgressMessage(std::string const &message) = 0;

  protected:
    
    int _taskProgressMin, _taskProgressMax;

  };

}

#endif
