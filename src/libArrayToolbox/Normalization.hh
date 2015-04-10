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

#ifndef ATBNORMALIZATION_HH
#define ATBNORMALIZATION_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "RuntimeError.hh"

#include <blitz/array.h>

namespace atb
{

  enum NormType { MINMAX, STDDEV, ZERO_MEAN_STDDEV };

/*======================================================================*/
/*! 
 *   Normalize a blitz::Array
 *
 *   \param data     The Array to normalize the gray values
 *   \param dataNorm The Array the normalized data will be stored to.
 *                   You may give the same Array for data and dataNorm!
 *   \param t        Normalization Type, one of:
 *                   MINMAX, STDDEV, ZERO_MEAN_STDDEV
 *
 *   \exception ArrayToolsError
 */
/*======================================================================*/
  template<typename ScalarT, int Dim>
  void
  normalize(blitz::Array<ScalarT,Dim> const &data,
            blitz::Array<ScalarT,Dim> &dataNorm,
            NormType t);

}

#include "Normalization.icc"

#endif
