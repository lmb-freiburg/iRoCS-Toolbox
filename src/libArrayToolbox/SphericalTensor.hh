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

/*======================================================================*/
/*!
 *  \file SphericalTensor.hh
 *  \brief Structures and functions for spherical tensor extraction and
 *    arithmetics.
 */
/*======================================================================*/

#ifndef SPHERICALTENSOR_HH
#define SPHERICALTENSOR_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <map>
#include <omp.h>

#include <libBlitzHdf5/FileIOWrapper.hh>

#include "Array.hh"

namespace atb
{
  
/*======================================================================*/
/*! 
 *   \struct SDMagFeatureIndex SphericalTensor.hh "libArrayToolbox/SphericalTensor.hh"
 *   \brief Data structure to index a feature structure consisting of
 *   multi-scale spherical derivative features.
 */
/*======================================================================*/
  struct SDMagFeatureIndex
  {

/*======================================================================*/
/*!
 *   \brief Create a new feature index for a rotation-invariant spherical
 *     tensor magnitude feature.
 *
 *   A spherical tensor magnitude feature is the magnitude of the selected
 *   tensor band of the Gaussian-windowed Schmidt semi-normalized solid
 *   harmonic function on the Gaussian and Laplace-filtered input volume.
 *
 *   \param sigma   The standard deviation of the Gaussian kernel for scale
 *     selection
 *   \param laplace The number of applied Laplacian filters before computing
 *     the spherical tensor
 *   \param band    The band of the spherical tensor expansion
 *   
 */
/*======================================================================*/
    SDMagFeatureIndex(const double sigma, const int laplace, const int band)
            : s(sigma), l(laplace), b(band)
          {}

/*======================================================================*/
/*!
 *   \brief Destructor.
 */
/*======================================================================*/
    ~SDMagFeatureIndex()
          {}

/*! Standard deviation of the Gaussian fliter used for scale selection */
    double s;

/*! The Laplace order of the spherical feature (l Laplacian filters are
 *  applied prior to spherical tensor computation)
 */
    int l;

/*! The spherical tensor band to compute the magnitude for */
    int b;

/*!
 *   \brief Strict weak ordering of spherical tensors for sorting.
 *
 *   Spherical tensors will be sorted by sigma, then l and finally b
 */
    bool operator<(const SDMagFeatureIndex& rhs) const
          {
            if (s < rhs.s) return true;
            if (s == rhs.s && l < rhs.l) return true;
            if (s == rhs.s && l == rhs.l && b < rhs.b) return true;
            return false;
          }

  };

  std::ostream &operator<<(std::ostream &os, SDMagFeatureIndex const &index);

/*======================================================================*/
/*! 
 *   \brief Compute real spherical derivatives of the tensor with given
 *     rank (band).
 *
 *   (c) 2010 Marco Reisert, University of Freiburg.
 *
 *   Remarks:
 *
 *   <ul>
 *     <li>complex numbers are stored interleaved.</li>
 *     <li>rank 0 tensor has also to be complex, i.e. of real dimension
 *       (2, sz[0],sz[1],sz[2])</li>
 *     <li>m-indices of spherical tensor are negative and reversed,
 *         i.e. -(L-1),-(L-2)...,0</li>
 *   </ul>
 *
 *   \param in      volume of size (L*2 , sz[0], sz[1], sz[2])
 *   \param sz      size array of length 3
 *   \param L       rank of input, rank of spherical tensor is L-1
 *   \param out     preallocated output volume of size 
 *                  ((L+1)*2,  sz[0], sz[1], sz[2])
 *   \param factor  result is multiplied by this real factor
 */
/*======================================================================*/
  void STderivReal(
      double *in, BlitzIndexT sz[], BlitzIndexT L, double *out,
      double factor = 1.0);

/*======================================================================*/
/*! 
 *   \brief Wrapper method for computing spherical tensorial derivative
 *   magnitude features.
 *
 *   \param data    The input Array to compute voxelwise features for
 *   \param out     The features sorted by the SDMagFeatureIndex
 *   \param sigma   The current feature scale (only for SDMagFeatureIndex)
 *   \param laplace The number of laplacians that have been applied before
 *                  (only for SDMagFeatureIndex)
 *   \param maxBand The maximum expansion band
 */
/*======================================================================*/
  template<typename DataT>
  void STderiv(const blitz::Array<DataT,3>& data,
               std::map<SDMagFeatureIndex,blitz::Array<DataT,3>*>& out,
               const double sigma, const int laplace,
               const BlitzIndexT maxBand);

/*======================================================================*/
/*! 
 *   \brief Wrapper method for computing spherical tensorial derivative
 *   magnitude features
 *
 *   \param data    The input Array to compute voxelwise features for
 *   \param out     The features sorted by the SDMagFeatureIndex
 *   \param sigma   The current feature scale (only for SDMagFeatureIndex)
 *   \param laplace The number of laplacians that have been applied before
 *                  (only for SDMagFeatureIndex)
 *   \param maxBand The maximum expansion band
 */
/*======================================================================*/
  template<typename DataT>
  void STderiv(Array<DataT,3> const &data,
               std::map<SDMagFeatureIndex,Array<DataT,3>*> &out,
               double sigma, int laplace, BlitzIndexT maxBand);

}

#include "SphericalTensor.icc"

#endif
