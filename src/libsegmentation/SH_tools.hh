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

#ifndef SHTOOLS_HH
#define SHTOOLS_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <vector>
#include <blitz/array.h>
#include <gsl/gsl_sf_legendre.h>

namespace segmentation
{

  typedef blitz::Array<std::complex<double>,2> SpatialArray;
  typedef blitz::Array<std::complex<double>,1> FrequencyArray;
  typedef std::vector<SpatialArray*> SpatialVecArray;
  typedef std::vector<FrequencyArray*> FrequencyVecArray;

  ptrdiff_t thetaToIndex(double theta, int bw);

  ptrdiff_t phiToIndex(double phi, int bw);

  double indexToTheta(ptrdiff_t index, int bw);

  double indexToPhi(ptrdiff_t index, int bw);

  blitz::TinyVector<double,2> indexToAngles(
      blitz::TinyVector<ptrdiff_t,2> const &index, int bw);

// Compute the linear index in a complex-valued array for the given bandwidth,
// band and order (To get the position in the corresponding interleaved float
// Array, multiply by two)
  ptrdiff_t linearIndex(int bw, int l, int m);

  void SH_dtheta_up(double const *c, int bw, double * d_theta);

  void SH_dtheta(FrequencyArray const &c, SpatialArray &d_theta,
                 int degree);

  void SH_dtheta(double const *c, int bw, double *d_theta, int degree = 1);

  void SH_dpsi(double const *c, int bw, double *d_psi);

  void SH_normal(double const *cx, double const *cy ,double const *cz,
                 double *nx, double *ny, double *nz, int bw);

  void SH_normal(double const *cx, double *nx, double *ny, double *nz,  int bw);

  void SH_curvature(double const *cx, double *curvature, int bw);

  float SH_volume(double const *cx, int bw);

  void SH_curvature(double const *cx, double const *cy ,double const *cz,
                    double *curvature, int bw);

  void SH_curvature_gaussian(double const *cx, double const *cy ,
                             double const *cz, double *curvature, int bw);

  void SH_curvature_hesse(double const *cx, double const *cy ,double const *cz,
                          double *curvature, int bw);

}

#endif
