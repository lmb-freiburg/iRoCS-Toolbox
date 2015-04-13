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

#ifndef LAYERASSIGNMENTWORKER_HH
#define LAYERASSIGNMENTWORKER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <libProgressReporter/ProgressReporter.hh>

#include <libArrayToolbox/Array.hh>
#include <libArrayToolbox/ATBNucleus.hh>
#include <libArrayToolbox/iRoCS.hh>

namespace iRoCS
{

  void assignLayers(
      atb::Array<double,3> const &data, std::vector<atb::Nucleus> &nuclei,
      atb::IRoCS const &iRoCS, std::string const &modelFileName,
      std::string const &cacheFileName = "",
      bool updateMitoses = false, bool cacheCoordinates = false,
      bool forceFeatureComputation = false, ProgressReporter *pr = NULL);

}

#endif
