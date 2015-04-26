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
