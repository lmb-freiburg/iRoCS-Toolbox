#ifndef ASSIGNLAYERSTOCELLSEGMENTATIONWORKER_HH
#define ASSIGNLAYERSTOCELLSEGMENTATIONWORKER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <libArrayToolbox/Array.hh>
#include <libArrayToolbox/algo/ShellCoordinateTransform.hh>

#include <libProgressReporter/ProgressReporter.hh>

namespace iRoCS
{

  void trainLayerAssignmentToSegmentation(
      std::vector<std::string> const &infiles,
      std::string const &segmentationName, std::string const &sctName,
      double volumeThresholdUm3, std::string const &modelFileName,
      std::vector<std::string> const &featureFileNames,
      std::string const &featureGroup, std::vector<std::string> &featureNames,
      std::vector<std::string> const &labelFileNames,
      std::string const &labelName, blitz::TinyVector<int,2> const &labelRange,
      bool computeFeatures, int backgroundLabel = 1,
      ProgressReporter *pr = NULL);

  void assignLayersToSegmentation(
      atb::Array<int,3> const &L, ShellCoordinateTransform const &sct,
      double volumeThresholdUm3, std::string const &modelFileName,
      std::string const &featureFileName, std::string const &featureGroup,
      std::vector<std::string> &featureNames,
      std::string const &outFileName, std::string const &labelName,
      int backgroundLabel = 1, ProgressReporter *pr = NULL);

}

#endif
