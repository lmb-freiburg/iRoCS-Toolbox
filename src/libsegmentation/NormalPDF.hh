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

#ifndef NORMALPDF_HH
#define NORMALPDF_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <blitz/array.h>

namespace segmentation {

/*===========================================================================*/
/*!
 * Encapsulates a normal pdf
 */
/*===========================================================================*/
template<typename T>
class NormalPDF {
public:
  /**
   * Estimate PDF from given set of samples
   * @param samples: A 2d array of samples, extent(0) is the number of samples
   *                 extend(1) the dimensionality of the samples
   */
  NormalPDF(const blitz::Array<T, 2> &samples);
  NormalPDF(const blitz::Array<T, 1> &mean,
            const blitz::Array<T, 2> &information);

  blitz::Array<T, 1> mean_;
  blitz::Array<T, 2> covariance_;
  blitz::Array<T, 2> information_matrix_;
  
  /**
   * Calculates information_matrix_
   * don't use information_matrix_ before calling this method
   * */
  void invertCovariance();
};

template<typename T>
class MahalanobisDistance {
public:
  MahalanobisDistance(){};
  T operator()(const NormalPDF<T>& pdf, const blitz::Array<T, 1>& sample);
};

}

#endif // NORMALPDF_HH
