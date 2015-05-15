/**************************************************************************
 *
 * Copyright (C) 2015 Marco Reisert, Thorsten Falk
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

#ifndef SPHERICALTENSOR_CC
#define SPHERICALTENSOR_CC

#include "SphericalTensor.hh"

#include <omp.h>

namespace atb
{
  
  std::ostream &operator<<(std::ostream &os, SDMagFeatureIndex const &index)
  {
    os << "SDMagFeatureIndex(" << index.s << ", " << index.l << ", "
       << index.b << ")";
    return os;
  }

  void STderivReal(double *in, BlitzIndexT sz[], BlitzIndexT L,
                   double *out, double factor)
  {
    double *real_out = out;
    double *imag_out = out+1;
    double *real_in = in;
    double *imag_in = in+1;
    
    BlitzIndexT totsiz = sz[0]*sz[1]*sz[2];
    BlitzIndexT sz1 = sz[0];
    BlitzIndexT sz2 = sz[0]*sz[1];
    
    BlitzIndexT start = sz[1]*sz[0] + sz[0] + 1;
    BlitzIndexT end = totsiz-start;
    
    BlitzIndexT TL = L+1;
    BlitzIndexT TLS = L;
    for (BlitzIndexT ms = -L; ms <= 0 ;ms++)
    {
      double cg_m1 = factor * sqrt(
          static_cast<double>((L - ms) * (L - ms - 1)) /
          static_cast<double>((2 * L) * (2 * L - 1)));
      double cg_0 =  factor * sqrt(
          static_cast<double>(2 * (L + ms) * (L - ms)) /
          static_cast<double>(L * (2 * L - 1)));
      double cg_p1 = factor * sqrt(
          static_cast<double>((L + ms) * (L + ms - 1)) /
          static_cast<double>((2 * L) * (2 * L - 1)));
      
      BlitzIndexT m = 2 * (ms + (L - 1));

#ifdef _OPENMP
#pragma omp parallel for
#endif
      for (BlitzIndexT k = start;k < end;k++)
      {
        BlitzIndexT dx = 2*(k+1)*TLS;
        BlitzIndexT dx_ = 2*(k-1)*TLS;
        BlitzIndexT dy = 2*(k+sz1)*TLS;
        BlitzIndexT dy_ = 2*(k-sz1)*TLS;
        BlitzIndexT dz = 2*(k+sz2)*TLS;
        BlitzIndexT dz_ = 2*(k-sz2)*TLS;
  
        BlitzIndexT iout = 2*(k*TL+ms+L);
        
        real_out[iout] = 0;
        imag_out[iout] = 0;
        
        if (ms-1 >= -(L-1))
        {
          real_out[iout] += cg_p1 * ((real_in[dx+m-2] - real_in[dx_+m-2]) +
                                     (imag_in[dy+m-2] - imag_in[dy_+m-2]));
          imag_out[iout] += cg_p1 * ((imag_in[dx+m-2] - imag_in[dx_+m-2]) -
                                     (real_in[dy+m-2] - real_in[dy_+m-2]));
        }
        if (ms <= L-1 && ms >= -(L-1))		
        {
          real_out[iout] += cg_0  * (real_in[dz+m] - real_in[dz_+m]);
          imag_out[iout] += cg_0  * (imag_in[dz+m] - imag_in[dz_+m]);
        }
        if (ms+1 <= (L-1))
        {
          if (ms == 0)
          {
            real_out[iout] += cg_m1 * ((real_in[dx+m-2] - real_in[dx_+m-2]) +
                                       (imag_in[dy+m-2] - imag_in[dy_+m-2]));
            imag_out[iout] -= cg_m1 * ((imag_in[dx+m-2] - imag_in[dx_+m-2]) -
                                       (real_in[dy+m-2] - real_in[dy_+m-2]));
          }
          else
          {
            real_out[iout] -= cg_m1 * ((real_in[dx+m+2] - real_in[dx_+m+2]) -
                                       (imag_in[dy+m+2] - imag_in[dy_+m+2]));
            imag_out[iout] -= cg_m1 * ((imag_in[dx+m+2] - imag_in[dx_+m+2]) +
                                       (real_in[dy+m+2] - real_in[dy_+m+2]));
          }
        } 
      }
    }
  }
 
}

#endif
