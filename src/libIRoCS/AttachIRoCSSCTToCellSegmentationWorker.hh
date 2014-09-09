#ifndef ATTACHIROCSSCTTOCELLSEGMENTATIONWORKER_HH
#define ATTACHIROCSSCTTOCELLSEGMENTATIONWORKER_HH

#include <libProgressReporter/ProgressReporter.hh>

#include <libArrayToolbox/Array.hh>
#include <libArrayToolbox/algo/ShellCoordinateTransform.hh>

namespace iRoCS
{

  void boundaryDetection(
      blitz::Array<bool,3> const &segmentation, blitz::Array<bool,3> &border);

  void extractPosition(
      blitz::Array<bool,3> &shell,
      std::vector< blitz::TinyVector<double,3> > &shellPoints);

  void attachIRoCS(
      ShellCoordinateTransform &sct, atb::Array<int,3> const &segmentation,
      int backgroundLabel, int downSampleRatio, double segmentLength,
      double margin,
      blitz::TinyVector<double,3> const &posQC =
      blitz::TinyVector<double,3>(-std::numeric_limits<double>::infinity()),
      blitz::TinyVector<double,3> const &xDirection =
      blitz::TinyVector<double,3>(-std::numeric_limits<double>::infinity()),
      std::string const &debugFileName = "", 
      atb::Array<double,3> *l = NULL, atb::Array<double,3> *r = NULL,
      atb::Array<double,3> *p = NULL, iRoCS::ProgressReporter *pr = NULL);

}

#endif
