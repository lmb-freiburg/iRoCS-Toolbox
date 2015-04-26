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

#include "Random.hh"

namespace atb
{

  double gaussRandomNumber(const double mean, const double stddev) 
  {
    double x1, x2, w;
    do 
    {
      x1 = 2.0 * static_cast<double>(rand()) / RAND_MAX - 1.0;
      x2 = 2.0 * static_cast<double>(rand()) / RAND_MAX - 1.0;
      w = x1 * x1 + x2 * x2;
    }
    while (w >= 1.0);
    w = std::sqrt(-2.0 * std::log(w) / std::log(2.0) / w);
    return (x1 * w * stddev) + mean;
  }

}
