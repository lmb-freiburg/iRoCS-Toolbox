#include "ProgressReporterStream.hh"

namespace iRoCS
{

  ProgressReporterStream::ProgressReporterStream(
      std::ostream& os, int progress, int progressMin, int progressMax,
      std::string const &headerMessage, std::string const &progressMessage)
          : ProgressReporter(), _os(os), _progress(progress),
            _progressMin(progressMin), _progressMax(progressMax),
            _headerMessage(headerMessage), _progressMessage(progressMessage),
            _aborted(false)
  {}

  ProgressReporterStream::~ProgressReporterStream()
  {}

  void ProgressReporterStream::setProgressMin(int progressMin)
  {
#ifdef _OPENMP
#pragma omp critical (_PROGRESSMIN_IS_CURRENTLY_UPDATING_)
#endif
    _progressMin = progressMin;
  }

  int ProgressReporterStream::progressMin() const
  {
    return _progressMin;
  }

  void ProgressReporterStream::setProgressMax(int progressMax)
  {
#ifdef _OPENMP
#pragma omp critical (_PROGRESSMAX_IS_CURRENTLY_UPDATING_)
#endif
    _progressMax = progressMax;
  }

  int ProgressReporterStream::progressMax() const
  {
    return _progressMax;
  }

  void ProgressReporterStream::setHeaderMessage(std::string const &message)
  {
#ifdef _OPENMP
#pragma omp critical (_HEADERMESSAGE_IS_CURRENTLY_UPDATING_)
#endif
    _headerMessage = message;
  }

  std::string ProgressReporterStream::headerMessage() const
  {
    return _headerMessage;
  }

  void ProgressReporterStream::setProgressMessage(std::string const &message)
  {
#ifdef _OPENMP
#pragma omp critical (_PROGRESSMESSAGE_IS_CURRENTLY_UPDATING_)
#endif
    _progressMessage = message;
  }

  std::string ProgressReporterStream::progressMessage() const
  {
    return _progressMessage;
  }
  
  void ProgressReporterStream::setProgress(int progress)
  {
#ifdef _OPENMP
#pragma omp critical (_PROGRESS_IS_CURRENTLY_UPDATING_)
#endif
    _progress = progress;
  }
  
  int ProgressReporterStream::progress() const
  {
    return _progress;
  }

  void ProgressReporterStream::setAborted(bool abort)
  {
#ifdef _OPENMP
#pragma omp critical (_ABORTED_IS_CURRENTLY_UPDATING_)
#endif
    _aborted = abort;
  }

  void ProgressReporterStream::abort()
  {
#ifdef _OPENMP
#pragma omp critical (_ABORTED_IS_CURRENTLY_UPDATING_)
#endif
    _aborted = true;
  }

  void ProgressReporterStream::abortWithError(std::string const &msg)
  {
#ifdef _OPENMP
#pragma omp critical (_ABORTED_IS_CURRENTLY_UPDATING_)
#endif
    {
      _aborted = true;
      std::cerr << msg << std::endl;
    }
  }

  bool ProgressReporterStream::isAborted() const
  {
    return _aborted;
  }

  bool ProgressReporterStream::updateProgress(int progress)
  {
#ifdef _OPENMP
#pragma omp critical (_PROGRESS_IS_CURRENTLY_UPDATING_)
#endif
    {
      if (_progress != progress)
      {
        _progress = progress;
        _os << _headerMessage << " "
            << 100 * static_cast<double>(_progress - _progressMin) /
            (_progressMax - _progressMin) << "% done"
            << std::flush;
        if (progress == _progressMax) _os << std::endl;
      }
    }
    return !_aborted;
  }

  bool ProgressReporterStream::updateProgressMessage(std::string const &message)
  {
#ifdef _OPENMP
#pragma omp critical (_PROGRESSMESSAGE_IS_CURRENTLY_UPDATING_)
#endif
    {
      if (message != _progressMessage)
      {
        _progressMessage = message;
        _os << _progressMessage << std::endl;
      }
    }
    return !_aborted;
  }

}
