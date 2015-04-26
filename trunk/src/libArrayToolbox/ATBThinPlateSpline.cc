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

#include "ATBThinPlateSpline.hh"

#if defined HAVE_CONFIG_H && !defined PACKAGE
#include <config.hh>
#endif

#ifdef HAVE_BLITZ_V9
#include <blitz/tinyvec-et.h>
#endif

namespace atb
{
  
  template<>
  double tpsDistance(
      const blitz::TinyVector<double,1>& p1,
      const blitz::TinyVector<double,1>& p2)
  {
    double d = std::abs(p1(0) - p2(0));
    return d * d * d;
  }

  template<>
  double tpsDistance(
      const blitz::TinyVector<double,2>& p1,
      const blitz::TinyVector<double,2>& p2)
  {
    double n = std::sqrt(blitz::dot(p1 - p2, p1 - p2));
    return n * std::log(n);
  }

  template<>
  double tpsDistance(
      const blitz::TinyVector<double,3>& p1,
      const blitz::TinyVector<double,3>& p2)
  {
    return std::sqrt(blitz::dot(p1 - p2, p1 - p2));
  }

}

