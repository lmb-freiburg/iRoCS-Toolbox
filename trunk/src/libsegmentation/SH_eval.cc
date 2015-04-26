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

#include "SH_eval.hh"

namespace segmentation 
{

  std::complex<double> evaluateSH(
      FrequencyArray const &coeffs, double theta, double phi)
  {
    std::complex<double> res = 0.0;
    int bw = static_cast<int>(std::sqrt(static_cast<double>(coeffs.size())));
    for (int l = 0; l < bw; ++l)
    {
      for (int m = -l; m < 0; ++m)
      {
        res += coeffs(linearIndex(bw, l, m)) *
            ((-m % 2 == 0) ? 1.0 : -1.0) *
            gsl_sf_legendre_sphPlm(l, -m, std::cos(theta)) *
            std::exp(
                std::complex<double>(0.0, 1.0) * static_cast<double>(m) * phi);
      }
      for (int m = 0; m <= l; ++m)
      {
        res += coeffs(linearIndex(bw, l, m)) *
            gsl_sf_legendre_sphPlm(l, m, std::cos(theta)) *
            std::exp(
                std::complex<double>(0.0, 1.0) * static_cast<double>(m) * phi);
      }
    }
    return res;
  }

}
