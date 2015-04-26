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

#ifndef ATTACHIROCSSCTTOCELLSEGMENTATIONWORKER_HH
#define ATTACHIROCSSCTTOCELLSEGMENTATIONWORKER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

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
