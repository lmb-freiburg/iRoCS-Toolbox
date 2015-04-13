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

#ifndef ATTACHIROCSWORKER_HH
#define ATTACHIROCSWORKER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <libProgressReporter/ProgressReporter.hh>
#include <libArrayToolbox/iRoCS.hh>

namespace iRoCS
{

  void attachIRoCS(
      atb::IRoCS &iRoCS, std::vector<atb::Nucleus> &nuclei,
      blitz::TinyVector<double,3> const &qcPositionUm, double kappa = 1.0,
      double lambda = 0.0, double mu = 0.0, double searchRadiusUm = 0.0,
      int nIterations = 1000, double tau = 0.1, ProgressReporter *pr = NULL);

}

#endif
