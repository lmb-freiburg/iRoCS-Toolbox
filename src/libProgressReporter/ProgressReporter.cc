#include "ProgressReporter.hh"

namespace iRoCS
{
  
  void ProgressReporter::setTaskProgressRange(int progressMin, int progressMax)
  {
    _taskProgressMin = progressMin;
    _taskProgressMax = progressMax;
  }
  
  void ProgressReporter::setTaskProgressMin(int progressMin)
  {
    _taskProgressMin = progressMin;
  }
  
  int ProgressReporter::taskProgressMin() const
  {
    return _taskProgressMin;
  }
  
  void ProgressReporter::setTaskProgressMax(int progressMax)
  {
    _taskProgressMax = progressMax;
  }
  
  int ProgressReporter::taskProgressMax() const
  {
    return _taskProgressMax;
  }
  
}

