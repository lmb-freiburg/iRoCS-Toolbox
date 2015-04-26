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

#ifndef LIBSEGMENTATION_SRC_PROFILESAMPLER_HH
#define LIBSEGMENTATION_SRC_PROFILESAMPLER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <blitz/array.h>
#include <vector>
#include <libProgressReporter/ProgressReporter.hh>

namespace segmentation
{

// forward declarations
  template<typename T>
  class MahalanobisDistance;

  template<class T, int Dim>
  class ProfileSampler
  {

  public:

    /**
     * @param numSamples: number of samples
     * @param samplingDistUm: sampling distance in micrometers
     */
    ProfileSampler(int numSamples, T samplingDistUm);
  
    /**
     * @param data: Data to sample from
     * @param elSize: element size in micrometers
     * @param centerUm: cell center in micrometers
     * @param positionUm: sampling position in micrometers
     * @returns: samples array of size num_samples - 1
     */
    blitz::Array<T,1> sample(
        blitz::Array<T,Dim> const &data,
        blitz::TinyVector<double,Dim> const &elSize,
        blitz::TinyVector<double,Dim> const &centerUm,
        blitz::TinyVector<double,Dim> const &positionUm) const;

    /**
     * @param data: Data to sample from
     * @param elSize: element size in micrometers
     * @param centerUm: cell center in micrometers
     * @param candidates: vector of positions where to sample
     * @param profiles: output array, will be resized automatically
     */
    void sample(
        blitz::Array<T,Dim> const &data,
        blitz::TinyVector<double,Dim> const &elSize,
        blitz::TinyVector<double,Dim> const &centerUm,
        std::vector< blitz::TinyVector<double,Dim> > const &candidates,
        blitz::Array<T, 2> &profiles,
        iRoCS::ProgressReporter *progress = NULL) const;

    /**
     * @param data: Data to sample from
     * @param elSize: element size in micrometers
     * @param candidates: if values in canidates is >= threshold, a profile
     * is sampled at its position
     * @param threshold: see profiles
     * @param profiles: output array, will be resized automatically
     * @param used_canidates: output vector of positions that where used for
     * sampling (candidates with value >= threshold)
     */
    void sample(
        blitz::Array<T,Dim> const &data,
        blitz::TinyVector<double,Dim> const &elSize,
        blitz::TinyVector<double,Dim> const &centerUm,
        blitz::Array<T,Dim> const &candidates, T threshold,
        blitz::Array<T,2> &profiles,
        std::vector< blitz::TinyVector<double,Dim> > &usedCandidates,
        blitz::TinyVector<double,Dim> const &lbUm = 0.0,
        blitz::TinyVector<double,Dim> const &ubUm =
        (std::numeric_limits<T>::infinity()),
        iRoCS::ProgressReporter *progress = NULL) const;

  private:

    int _numSamples;
    T _samplingDistUm;

  };

} // segmentation

#endif //LIBSEGMENTATION_SRC_PROFILESAMPLER_HH
