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

#ifndef DETECTSPHERESWORKER_HH
#define DETECTSPHERESWORKER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <libProgressReporter/ProgressReporter.hh>

#include <libArrayToolbox/HoughTransform.hh>
#include <libArrayToolbox/ATBNucleus.hh>

namespace iRoCS
{

  void detectSpheres(
      atb::Array<float,3> const &data, std::vector<atb::Nucleus> &spheres,
      atb::Array<float,3> &response, atb::Array<float,3> &radiusUm,
      blitz::TinyVector<double,2> const &radiusRangeUm, double radiusStepUm,
      double preSmoothingSigmaUm, double postSmoothingSigmaUm,
      double minMagnitude = 0.0, bool invertGradients = false,
      double gamma = 1.0, ProgressReporter *pr = NULL);

}

#endif
