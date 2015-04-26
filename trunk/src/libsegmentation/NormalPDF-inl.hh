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

#ifndef NORMALPDF_INL_HH
#define NORMALPDF_INL_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "NormalPDF.hh"

#include <libArrayToolbox/TypeTraits.hh>
#include <libArrayToolbox/ATBLinAlg.hh>

namespace segmentation {

// NormalPdf
template<typename T>
NormalPDF<T>::NormalPDF(
  const blitz::Array< T, 2 >& samples
) {
  atb::BlitzIndexT dim = samples.extent(1);
  mean_.resize(dim);
  covariance_.resize(dim, dim);
  mean_ = 0;
  covariance_ = 0;
  blitz::Range all = blitz::Range::all();

  atb::BlitzIndexT num_samples = samples.extent(0);

  for (int i = 0; i < num_samples; ++i)
    // add to mean
    mean_(all) += samples(i, all);

  mean_ /= (T)num_samples;

  for (int i = 0; i < num_samples; ++i) {
    // estimate covariance
    blitz::Array<T, 1> sample_demeaned(dim);
    sample_demeaned = samples(i, all) - mean_;
    for (int cov_row = 0; cov_row < dim; ++cov_row)
      for (int cov_col = cov_row; cov_col < dim; ++cov_col) {
        covariance_(cov_row, cov_col) += sample_demeaned(cov_row) *
                                         sample_demeaned(cov_col);
        // symetric
        covariance_(cov_col, cov_row) = covariance_(cov_row, cov_col);
      }
  }
  covariance_ /= (T)num_samples;
}

template<typename T>
NormalPDF<T>::NormalPDF(
  const blitz::Array<T, 1> &mean,
  const blitz::Array<T, 2> &information
) : mean_(mean), information_matrix_(information)  {
}

template<typename T>
void NormalPDF<T>::invertCovariance() {
  atb::invert(covariance_, information_matrix_);
}

template<>
void NormalPDF<float>::invertCovariance() {
  blitz::Array<double, 2> cov_d(blitz::cast<double>(covariance_));
  blitz::Array<double, 2> inf_d(covariance_.shape());
  atb::invert(cov_d, inf_d);
  information_matrix_.resize(covariance_.shape());
  information_matrix_ = blitz::cast<float>(inf_d);
}



template<typename T>
T MahalanobisDistance<T>::operator()(
  const NormalPDF<T>& pdf,
  const blitz::Array<T, 1>& sample) {
  blitz::Array<T, 1> sample_demeaned(pdf.mean_.extent(0));
  sample_demeaned = sample - pdf.mean_;
  // sample * information matrix
  blitz::Array<T, 1> sample_inf = atb::mvMult(pdf.information_matrix_,
                                  sample_demeaned);
  // mahalanobis distance
  T dot_product =  (T)atb::dot(sample_inf, sample_demeaned);
  return std::sqrt(dot_product);
}

} //namespace

#endif // NORMALPDF_INL_HH
