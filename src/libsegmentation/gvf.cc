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

#include "gvf-inl.hh"

// explicit template instanciations

namespace segmentation
{

  template
  void gradientVectorFlowSOR(
      blitz::Array<blitz::TinyVector<float, 3>, 3> &gradient,
      blitz::TinyVector<float,3> const &el_size_um, float mu, float nu,
      int max_iter, iRoCS::ProgressReporter *progress);
  
  template
  void gradientVectorFlowSOR(
      blitz::Array<blitz::TinyVector<double, 3>, 3> &gradient,
      blitz::TinyVector<double,3> const &el_size_um, double mu, double nu,
      int max_iter, iRoCS::ProgressReporter *progress);

  template
  void msGradientVectorFlow(
      blitz::Array<blitz::TinyVector<float, 3>, 3> &gradient,
      blitz::TinyVector<float,3> const &el_size_um, float mu, float hs,
      float hr, int max_iter, iRoCS::ProgressReporter *progress);
  
  template
  void msGradientVectorFlow(
      blitz::Array<blitz::TinyVector<double, 3>, 3> &gradient,
      blitz::TinyVector<double,3> const &el_size_um, double mu, double hs,
      double hr, int max_iter, iRoCS::ProgressReporter *progress);

}
