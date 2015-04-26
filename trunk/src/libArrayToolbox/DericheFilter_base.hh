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

/*======================================================================*/
/*!
 *  \file DericheFilter_base.hh
 *  \brief Implementation of the basic Deriche filter function approximating
 *    Gaussian filtering using recursion (IIR).
 */
/*======================================================================*/

#ifndef ATBDERICHEFILTER_BASE_HH
#define ATBDERICHEFILTER_BASE_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <libProgressReporter/ProgressReporter.hh>

#include "TypeTrais.hh"
#include "Interpolator.hh"

namespace atb
{
  
  template<typename DataT, int Dim>
  void filterDericheAlongDim(
      blitz::Array<DataT,Dim> const &data,
      blitz::TinyVector<double,Dim> const &elementSizeUm,
      blitz::Array<DataT,Dim> &filtered,
      double sigmaUm, int dim,
      BoundaryTreatmentType btType = ValueBT,
      DataT const &boundaryValue = traits<DataT>::zero,
      iRoCS::ProgressReporter *pr = NULL);

}

#include "DericheFilter_base.icc"

#endif
