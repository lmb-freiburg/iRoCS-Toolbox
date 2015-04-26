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

#ifndef LIBSEGMENTATION_GVF_HH
#define LIBSEGMENTATION_GVF_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <blitz/array.h>

#include <libProgressReporter/ProgressReporter.hh>

namespace segmentation
{

/**
 * Solve Euler-Lagrange equation for gradient vector flow using
 * successive over-relaxation
 * \f[
 * 0 = \mu \Delta u_i - \|\nabla f\|^2 ( u_i - \frac{\partial f}{\partial i} )
 * \f]
 * u : output gradient vector flow
 * f : input vector field
 * */
  template<typename T>
  void gradientVectorFlowSOR(
      blitz::Array<blitz::TinyVector<T, 3>, 3> &gradient,
      blitz::TinyVector<T,3> const &el_size_um, T mu, T nu, int max_iter,
      iRoCS::ProgressReporter *progress = NULL);

  template<typename T>
  void msGradientVectorFlow(
      blitz::Array<blitz::TinyVector<T, 3>, 3> &gradient,
      blitz::TinyVector<T,3> const &el_size_um, T mu, T hs, T hr, int max_iter,
      iRoCS::ProgressReporter *progress = NULL);

}

#endif // LIBSEGMENTATION_GVF_HH
