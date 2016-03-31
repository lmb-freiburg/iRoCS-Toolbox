/**************************************************************************
 *
 * Copyright (C) 2015 Margret Keuper, Thorsten Falk
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

#ifndef LIBMARGRET_EDGE_FILTER_H
#define LIBMARGRET_EDGE_FILTER_H

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <blitz/array.h>
#include <vector>
#include <libProgressReporter/ProgressReporter.hh>

namespace segmentation
{

/*!
 * calculate gradient magnitude with non-maximum-supression
 * \param data: input dataset
 * \param elSize: sampling size in micrometers
 * \param result: gradient magnitude with non-maximum supression
 **/
  template<class DataT>
  void edgeFilter(
      blitz::Array<DataT,3> const &data,
      blitz::TinyVector<double,3> const &elSize,
      blitz::Array<DataT,3> &result,
      blitz::TinyVector<double,3> const &lbUm = 0.0,
      blitz::TinyVector<double,3> const &ubUm =
      (std::numeric_limits<double>::infinity()),
      iRoCS::ProgressReporter *progress = NULL);

  template<class DataT>
  blitz::Array<DataT,3> _calculateGradients(
      blitz::Array<DataT,3> const &data,
      blitz::TinyVector<double,3> const &elSize,
      int direction, double scaling,
      iRoCS::ProgressReporter *progress = NULL);

}

#include "EdgeFilter-inl.hh"

#endif

