/**************************************************************************
 *
 * Euclidean Distance Transform
 *
 *    This file provides a class to store microscopic datasets with
 *    element size and a transformation to apply. It provides a function
 *    valueAt() to first apply the transformation and then access
 *    the transformed position using the specified ArrayInterpolator.
 *
 * This file belongs to the iRoCS Toolbox.
 *
 * Copyright (C) 2008-2018 Thorsten Falk
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
 *  \file DistanceTransform.hh
 *  \brief DistanceTransform class computes the euclidean distance
 *    transform on binary blitz::Arrays.
 */
/*======================================================================*/

#ifndef ATBDISTANCETRANSFORM_HH
#define ATBDISTANCETRANSFORM_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <libProgressReporter/ProgressReporter.hh>

#include "Array.hh"

namespace atb {

/*======================================================================*/
/*!
 *   \class EuclideanDistanceTransformBase DistanceTransform.hh "libArrayToolbox/DistanceTransform.hh"
 *   \brief Exact 1-D euclidean distance transform using lower parabola
 *     embedding [1].
 *
 *   This line transform allows to compute distance transforms of arbitrary-
 *   dimensional data by subsequent dimension-wise application.
 *
 *   [1] P. Felzenszwalb and D. Huttenlocher. Theory of Computing, Vol. 8,
 *       No. 19, September 2012
 */
/*======================================================================*/
  struct EuclideanDistanceTransformBase
  {

/*======================================================================*/
/*!
 *  \enum Mode DistanceTransform.hh "libArrayToolbox/DistanceTransform.hh"
 *  \brief The Mode enum specifies whether the distance to foreground or
 *    background pixels is computed.
 */
/*======================================================================*/
    enum Mode {
        /** Compute distance to nearest foreground pixel */
        DistanceToForeground,
        /** Compute distance to nearest background pixel */
        DistanceToBackground };

/*======================================================================*/
/*!
 *   Assign background pixels this value and foreground pixels value 0.
 */
/*======================================================================*/
    static double BG_VALUE;

/*======================================================================*/
/*!
 *   Create a new EuclideanDistanceTransformBase object with given maximum
 *   line length
 *
 *   \param capacity The maximum line length to process
 */
/*======================================================================*/
    EuclideanDistanceTransformBase(int capacity);

/*======================================================================*/
/*!
 *   Destructor
 */
/*======================================================================*/
    ~EuclideanDistanceTransformBase();

/*======================================================================*/
/*!
 *   Apply the line transform to data stored in f. After processing,
 *   d contains the squared euclidean distances, _v contains the locations
 *   of the angular points of the parabola and _z the intervals on which the
 *   corresponding parabola has smallest value, i.e. values of parabola i
 *   with angular point (_v[i], f(_v[i])) are smaller than values of all
 *   other parabola in interval (_z[i], _z[i+1]). Only parabola contributing
 *   to the lower envelope are contained.
 *
 *   \param n The length of the line
 */
/*======================================================================*/
    void run(int n);

/*======================================================================*/
/*!
 *   The input to process the distance transform for. Can be a raw binary
 *   1-D Array containing 0 for foreground and BG_VALUE for background or
 *   the result of previous line transforms in orthogonal dimensions for
 *   n-D data.
 */
/*======================================================================*/
    std::vector<double> f;

/*======================================================================*/
/*!
 *   The squared eucliden distances of f
 */
/*======================================================================*/
    std::vector<double> d;

/*======================================================================*/
/*!
 *   The intervals of parabola contributing to the lower distance envelope
 */
/*======================================================================*/
    std::vector<double> _z;

/*======================================================================*/
/*!
 *   The angular points of parabola contributing to the lower distance
 *   envelope
 */
/*======================================================================*/
    std::vector<double> _v;

  };

/*======================================================================*/
/*!
 *  \class DistanceTransform DistanceTransform.hh "libArrayToolbox/DistanceTransform.hh"
 *  \brief The DistanceTransform class computes the exact euclidean
 *    distance transform of binary images.
 */
/*======================================================================*/
  template<typename DataT, int Dim>
  class DistanceTransform
  {

  public:

/*======================================================================*/
/*!
 *   Binary n-D euclidean distance transform. The given fg value is treated
 *   as foreground, all other values as background.
 *
 *   \param distances The euclidean distances to either foreground or
 *     background
 *   \param data The Array to compute the distance transform for
 *   \param mode Distances can be computed to foreground pixels
 *     (EuclideanDistanceTransformBase.DistanceToForeground) or to
 *     background pixels (EuclideanDistanceTransformBase.DistanceToBackground)
 *   \param fg   The value of foreground pixels
 *   \param pr   A progress reporter to output progress to
 */
/*======================================================================*/
    static void getDistanceInPixels(
        Array<double,Dim> &distances,
        Array<DataT,Dim> const &data,
        EuclideanDistanceTransformBase::Mode mode =
        EuclideanDistanceTransformBase::DistanceToForeground,
        DataT fg = traits<DataT>::one, iRoCS::ProgressReporter *pr = NULL);

/*======================================================================*/
/*!
 *   Binary n-D euclidean distance transform. The given fg value is treated
 *   as foreground, all other values as background.
 *
 *   \param nearestPixels The output Array returning the position of the
 *     nearest foreground/background pixel
 *   \param data The Array to compute the distance transform for
 *   \param mode Distances can be computed to foreground pixels
 *     (EuclideanDistanceTransformBase.DistanceToForeground) or to
 *     background pixels (EuclideanDistanceTransformBase.DistanceToBackground)
 *   \param fg   The value of foreground pixels
 *   \param pr   A progress reporter to output progress to
 */
/*======================================================================*/
    static void getNearestPixels(
        Array<blitz::TinyVector<BlitzIndexT,Dim>,Dim> &nearestPixels,
        Array<DataT,Dim> const &data,
        EuclideanDistanceTransformBase::Mode mode =
        EuclideanDistanceTransformBase::DistanceToForeground,
        DataT fg = traits<DataT>::one, iRoCS::ProgressReporter *pr = NULL);

/*======================================================================*/
/*!
 *   Binary n-D euclidean distance transform. The given fg value is treated
 *   as foreground, all other values as background.
 *
 *   \param distances The euclidean distances to either foreground or
 *     background
 *   \param data The Array to compute the distance transform for
 *   \param elementSizeUm The voxel size in micrometers
 *   \param mode Distances can be computed to foreground pixels
 *     (EuclideanDistanceTransformBase.DistanceToForeground) or to
 *     background pixels (EuclideanDistanceTransformBase.DistanceToBackground)
 *   \param fg   The value of foreground pixels
 *   \param pr   A progress reporter to output progress to
 */
/*======================================================================*/
    static void getDistanceInPixels(
        blitz::Array<double,Dim> &distances,
        blitz::Array<DataT,Dim> const &data,
        blitz::TinyVector<double,Dim> const &,
        EuclideanDistanceTransformBase::Mode mode =
        EuclideanDistanceTransformBase::DistanceToForeground,
        DataT fg = traits<DataT>::one, iRoCS::ProgressReporter *pr = NULL);

/*======================================================================*/
/*!
 *   Binary n-D euclidean distance transform. The given fg value is treated
 *   as foreground, all other values as background.
 *
 *   \param nearestPixels The output Array returning the position of the
 *     nearest foreground/background pixel for every pixel
 *   \param data The Array to compute the distance transform for
 *   \param elementSizeUm The voxel size in micrometers
 *   \param mode Distances can be computed to foreground pixels
 *     (EuclideanDistanceTransformBase.DistanceToForeground) or to
 *     background pixels (EuclideanDistanceTransformBase.DistanceToBackground)
 *   \param fg   The value of foreground pixels
 *   \param pr   A progress reporter to output progress to
 */
/*======================================================================*/
    static void getNearestPixels(
        blitz::Array<blitz::TinyVector<BlitzIndexT,Dim>,Dim> &nearestPixels,
        blitz::Array<DataT,Dim> const &data,
        blitz::TinyVector<double,Dim> const &,
        EuclideanDistanceTransformBase::Mode mode =
        EuclideanDistanceTransformBase::DistanceToForeground,
        DataT fg = traits<DataT>::one, iRoCS::ProgressReporter *pr = NULL);

  };

}

#include "DistanceTransform.icc"

#endif
