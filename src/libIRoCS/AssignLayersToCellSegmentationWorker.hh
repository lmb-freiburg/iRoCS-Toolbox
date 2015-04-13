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
