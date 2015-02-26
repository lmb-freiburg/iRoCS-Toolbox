#ifndef ROOTSEGMENTATIONWORKER_HH
#define ROOTSEGMENTATIONWORKER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <libProgressReporter/ProgressReporter.hh>

#include <libArrayToolbox/Array.hh>

namespace iRoCS
{
  
  void varianceNormalization(
      blitz::Array<double,3> &data,
      blitz::TinyVector<double,3> const &elementSizeUm, double sigmaUm,
      double epsilon = 1e-10, iRoCS::ProgressReporter *pr = NULL);

  void segmentCells(
      atb::Array<double,3> &data, atb::Array<int,3> &segmentation, double gamma,
      int normalizationType, int medianWidthPx, double processingElementSizeUm,
      double varSigmaUm, double varEpsilon, float sigmaHessianUm,
      bool preDiffusion, int nDiffusionIterations, float zCompensationFactor,
      double kappa, float deltaT, float l1Threshold, float volumeThresholdUm,
      int boundaryThicknessPx, std::string const &debugFileName = "",
      iRoCS::ProgressReporter *pr = NULL);

}

#endif
