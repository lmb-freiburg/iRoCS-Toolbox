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

#ifndef SEGMENTATION_GEODESIC_HH
#define SEGMENTATION_GEODESIC_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#define USE_S2KIT
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include <blitz/array.h>

#include <fftw3.h>

#include <libProgressReporter/ProgressReporter.hh>

#include <libArrayToolbox/Interpolator.hh>

extern "C"
{
#include <lmbs2kit/makeweights.h>
#include <lmbs2kit/cospmls.h>
#include <lmbs2kit/FST_semi_memo.h>
#include <lmbs2kit/csecond.h>
#include <lmbs2kit/primitive.h>
#include <lmbs2kit/naive_synthesis.h>
#include <lmbs2kit/seminaive.h>
#include <lmbs2kit/pmls.h>
}

#include "SH_forward.hh"
#include "SH_backward.hh"
#include "SH_tools.hh"

namespace segmentation
{

/*======================================================================*/
/*! 
 *   Project the elements of the given vector field to rays originating
 *   from the given center point.
 *
 *   \param field         The vector field to project
 *   \param elementSizeUm The element size in micrometers
 *   \param result        The Array to store the projection of the vectors
 *   \param centerUm      The point the rays originate from
 */
/*======================================================================*/
  template<typename DataT, int Dim>
  void radialProjection(
      blitz::Array<blitz::TinyVector<DataT,Dim>,Dim> const &field,
      blitz::TinyVector<double,Dim> const &elementSizeUm,
      blitz::TinyVector<double,Dim> const &centerUm,
      blitz::Array<DataT,Dim> &result);

/*======================================================================*/
/*! 
 *   Evolve an active contour with spherical topology represented by a
 *   scalar spherical harmonic function, describing the structure's
 *   radius.
 *
 *   \param edgeForce     The force field guiding the contour in the data
 *     term
 *   \param elementSizeUm The element size of the used Arrays in micrometers
 *   \param coeffs        After evolution the final spherical harmonic
 *     coefficients will be written to the coeffs array
 *   \param positionUm    The center position of the initial sphere
 *   \param radiusUm      The radius of the initial sphere
 *   \param bw            The bandwidth up to which the spherical harmonics
 *     will be computed
 *   \param roundness     The weight for the roundness term. This is a value
 *     greater zero. The higher the value the worse curvature deviations from
 *     a sphere are penalized. The usual parameter range is between zero and
 *     one.
 *   \param nIter         The maximum number of iterations to apply
 *   \param tau           The update step width. 0.1 is stable and should not
 *     be increased. If you experience oscillations you can try to decrease
 *     this value on the expense of slower convergence
 *   \param progress      If you want to get feedback about the progress of
 *     the evolution you can attach an iRoCS::ProgressReporter. The fraction of
 *     current iteration over all iterations will be reported based on the
 *     progress reporter's task min and max progress. This is a worst case
 *     estimate. Usually the contour will converge to the solution and
 *     immediately return before nIter is reached.
 */
/*======================================================================*/
  template<typename DataT>
  void geodesicSH(
      blitz::Array<blitz::TinyVector<DataT,3>,3> const &edgeForce,
      blitz::TinyVector<double,3> const &elementSizeUm,
      FrequencyArray &coeffs, blitz::TinyVector<double,3> const &positionUm,
      double radiusUm, int bw, double roundness, int nIter, double tau = 0.1,
      iRoCS::ProgressReporter *progress = NULL);

/*======================================================================*/
/*! 
 *   Generate a binary mask being true inside the spherical function
 *   and false outside. The spherical function will be centered at the
 *   given center point. The mask Array has to be given in the appropriate
 *   shape.
 *
 *   \param coeffs        The SH coefficients describing the surface of the
 *     object in spherical topology.
 *   \param mask          The binary Array, the mask will be written to
 *   \param elementSizeUm The element size of the mask in micrometers
 *   \param centerUm      The center of the object to generate a mask from
 */
/*======================================================================*/
  void generateMask(
      FrequencyArray const &coeffs, blitz::Array<bool,3> &mask,
      blitz::TinyVector<double,3> const &elementSizeUm,
      blitz::TinyVector<double,3> const &centerUm);

/**
 * Geodesic Active contours
 * @param gvf: a gradient vector flow field
 * @param elementSizeUm: element size in um
 * @param coeffs The SH coefficients describing the evolved surface
 * @param positionUm: center of sphere used for initialization (in um)
 * @param radiusUm: radius of wphere used for initialization (in um)
 * @param bw: cutoff bandwidth
 * @param num_iter: number of gradient descent iterations 
 * */
  void geodesic_sh(
      blitz::Array<blitz::TinyVector< float, 3>, 3> const &gvf,
      blitz::TinyVector<float, 3> const &elementSizeUm,
      FrequencyArray &coeffs,
      blitz::TinyVector<float, 3> positionUm, float radiusUm,
      int bw, double roundness, int num_iter,
      iRoCS::ProgressReporter *progress = NULL);

/**
 * Geodesic Active contours
 * @param gvf: a gradient vector flow field
 * @param el_size_um: element size in um
 * @param best_center: center of sphere used for initialization (in um)
 * @param best_radius: radius of wphere used for initialization (in um)
 * @param bandwidth: cutoff bandwidth
 * @param num_iter: number of gradient descent iterations 
 * @param mask: output mask
 * @param normal: normals of mask
 * */
  void geodesic_sh(
      blitz::Array<blitz::TinyVector< float, 3>, 3> const &gvf,
      blitz::TinyVector<float, 3> const &el_size_um,
      blitz::TinyVector<float, 3> best_center,
      float best_radius, int bandwidth, double roundness,
      int num_iter, blitz::Array<bool, 3> &mask,
      blitz::Array<blitz::TinyVector< float, 3  >, 3> &normals,
      iRoCS::ProgressReporter *progress = NULL);

} //namespace

#include "geodesic.icc"

#endif // guard
