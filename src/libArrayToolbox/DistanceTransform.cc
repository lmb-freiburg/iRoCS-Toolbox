/**************************************************************************
 *
 * Euclidean Distance Transform
 *
 *    This file provides a class to store microscopic datasets with
 *    element size and a transformation to apply. It provides a function
 *    valueAt() to first apply the transformation and then access
 *    the transformed position using the specified ArrayInterpolator.
 *
 * This file belongs to the iRoCS Toolbox.
 *
 * Copyright (C) 2008-2018 Thorsten Falk
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

#include "DistanceTransform.hh"

namespace atb {

  double EuclideanDistanceTransformBase::BG_VALUE = 1.0e20;

  EuclideanDistanceTransformBase::EuclideanDistanceTransformBase(int capacity)
          : f(capacity), d(capacity), _z(capacity + 1), _v(capacity) {}

  EuclideanDistanceTransformBase::~EuclideanDistanceTransformBase() {}

  void EuclideanDistanceTransformBase::run(int n) {
    int k = 0;
    _v[0] = 0;
    _z[0] = -BG_VALUE;
    _z[1] = BG_VALUE;
    for (int q = 1; q < n; q++) {
      double s  = ((f[q] + q * q) - (f[_v[k]] + _v[k] * _v[k])) /
          (2 * q - 2 * _v[k]);
      while (s <= _z[k]) {
        k--;
        s = ((f[q] + q * q) - (f[_v[k]] + _v[k] * _v[k])) /
            (2 * q - 2 * _v[k]);
      }
      k++;
      _v[k] = q;
      _z[k] = s;
      _z[k+1] = BG_VALUE;
    }

    k = 0;
    for (int q = 0; q < n; q++) {
      while (_z[k+1] < q) k++;
      d[q] = (q - _v[k]) * (q - _v[k]) + f[_v[k]];
    }
  }

}
