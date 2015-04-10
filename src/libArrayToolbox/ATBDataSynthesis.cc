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

#include "ATBDataSynthesis.hh"

#include <vector>

namespace atb
{
  
  double erfbase(const double x, ErrorFunctionType type) 
  {
    double xbreak = 0.46875;
    double result = 0;

    // In this range direct computation of erf is more accurate
    if (fabs(x) <= xbreak) 
    {
      std::vector<double> a(5);
      a[0] = 3.16112374387056560e00;
      a[1] = 1.13864154151050156e02;
      a[2] = 3.77485237685302021e02;
      a[3] = 3.20937758913846947e03;
      a[4] = 1.85777706184603153e-1;
      std::vector<double> b(4);
      b[0] = 2.36012909523441209e01;
      b[1] = 2.44024637934444173e02;
      b[2] = 1.28261652607737228e03;
      b[3] = 2.84423683343917062e03;

      double y = fabs(x);
      double z = y * y;
      double xnum = a[4] * z;
      double xden = z;
      for (int i = 0; i < 3; ++i) 
      {
        xnum = (xnum + a[i]) * z;
        xden = (xden + b[i]) * z;
      }
      result = x * (xnum + a[3]) / (xden + b[3]);
      if (type != Erf) result = 1 - result;
      if (type == ErfCX) result = exp(z) * result;
    }

    // In this range direct computation of erfc is more accurate
    if (fabs(x) > xbreak && fabs(x) <= 4.0) 
    {
      std::vector<double> c(9);
      c[0] = 5.64188496988670089e-1;
      c[1] = 8.88314979438837594e00;
      c[2] = 6.61191906371416295e01;
      c[3] = 2.98635138197400131e02;
      c[4] = 8.81952221241769090e02;
      c[5] = 1.71204761263407058e03;
      c[6] = 2.05107837782607147e03;
      c[7] = 1.23033935479799725e03;
      c[8] = 2.15311535474403846e-8;
      std::vector<double> d(8);
      d[0] = 1.57449261107098347e01;
      d[1] = 1.17693950891312499e02;
      d[2] = 5.37181101862009858e02;
      d[3] = 1.62138957456669019e03;
      d[4] = 3.29079923573345963e03;
      d[5] = 4.36261909014324716e03;
      d[6] = 3.43936767414372164e03;
      d[7] = 1.23033935480374942e03;

      double y = fabs(x);
      double xnum = c[8] * y;
      double xden = y;
      double z = 0;
      double del = 0;
      for (int i = 0; i < 7; ++i)
      {
        xnum = (xnum + c[i]) * y;
        xden = (xden + d[i]) * y;
      }
      result = (xnum + c[7]) / (xden + d[7]);
      if (type != ErfCX) 
      {
        if (y >= 0) z = std::floor(y * 16) / 16;
        else z = std::ceil(y * 16) / 16;
        del = (y - z) * (y + z);
        result = exp(-z * z) * exp(-del) * result;
      }
    }

    // Another special case for large values of x
    if (fabs(x) > 4.0)
    {
      std::vector<double> p(6);
      p[0] = 3.05326634961232344e-1;
      p[1] = 3.60344899949804439e-1;
      p[2] = 1.25781726111229246e-1;
      p[3] = 1.60837851487422766e-2;
      p[4] = 6.58749161529837803e-4;
      p[5] = 1.63153871373020978e-2;
      std::vector<double> q(5);
      q[0] = 2.56852019228982242e00;
      q[1] = 1.87295284992346047e00;
      q[2] = 5.27905102951428412e-1;
      q[3] = 6.05183413124413191e-2;
      q[4] = 2.33520497626869185e-3;

      double y = std::abs(x);
      double z = 1.0 / (y * y);
      double xnum = p[5] * z;
      double xden = z;
      double del = 0;
      for (int i = 0; i < 4; ++i) 
      {
        xnum = (xnum + p[i]) * z;
        xden = (xden + q[i]) * z;
      }
      result = z * (xnum + p[4]) / (xden + q[4]);
      result = (1.0 / std::sqrt(M_PI) - result) / y;
      if (type != ErfCX) 
      {
        if (y >= 0) z = std::floor(y * 16) / 16;
        else z = std::ceil(y * 16) / 16;
        del = (y - z) * (y + z);
        result = exp(-z * z) * exp(-del) * result;
        if (std::abs(result) == std::numeric_limits<double>::infinity() ||
            result != result)
            result = 0;  // Remove NaN and infty
      }
    }
    
    if (type == Erf) 
    {
      if (x > xbreak) result = (0.5 - result) + 0.5;
      if (x < -xbreak) result = (-0.5 + result) - 0.5;
    }
    else if (type == ErfC) 
    {
      if (x < -xbreak) result = 2.0 - result;
    }
    else 
    {
      if (x < -xbreak) 
      {
        double z = std::ceil(x * 16) / 16;
        double del = (x - z) * (x + z);
        double y = exp(z * z) * exp(del);
        result = y + y - result;
      }
    }
    return result;
  }

}
