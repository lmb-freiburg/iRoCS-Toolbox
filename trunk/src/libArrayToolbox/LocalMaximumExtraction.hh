/**************************************************************************
 *
 * Copyright (C) 2015 Thorsten Falk
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

#ifndef ATBLOCALMAXIMUMEXTRACTION_HH
#define ATBLOCALMAXIMUMEXTRACTION_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <libProgressReporter/ProgressReporter.hh>

namespace atb
{

  enum NHood { SIMPLE_NHOOD, COMPLEX_NHOOD };

/*======================================================================*/
/*! 
 *   Extraction of local maxima of a blitz::Array
 *
 *   \param data        The blitz::Array to search maxima in
 *   \param localMaxima A list of TinyVectors, the local Minima are
 *                      appended to
 *   \param nh          Neighborhood to compare to. Possible values are\n
 *                      SIMPLE_NHOOD  - only direct neighbors along the
 *                                      axes (4-nhood in 2-D)\n
 *                      COMPLEX_NHOOD - all pixels having a common
 *                                      vertex with the current
 *                                      (8-nhood in 2-D, 26-nhood in 3-D)
 *   \param progress    If desired a progress reporter can be attached.
 *      All output will be redirected to the progress reporter and the
 *      progress will be updated regularly
 */
/*======================================================================*/
  template<typename Type, typename IndexT, int Dim>
  void
  extractLocalMaxima(blitz::Array<Type,Dim> const &data,
                     std::list< blitz::TinyVector<IndexT,Dim> > &localMaxima,
                     NHood nh = COMPLEX_NHOOD,
                     iRoCS::ProgressReporter *progress = NULL);

/*======================================================================*/
/*! 
 *   Extraction of local maxima of a blitz::Array
 *
 *   \param data        The blitz::Array to search maxima in
 *   \param localMaxima A list of TinyVectors, the local Minima are
 *                      appended to
 *   \param minValue    Only local maxima with value above minValue will
 *                      be added to the list (optional, if not given all
 *                      local maxima are returned)
 *   \param nh          Neighborhood to compare to. Possible values are\n
 *                      SIMPLE_NHOOD  - only direct neighbors along the
 *                                      axes (4-nhood in 2-D)\n
 *                      COMPLEX_NHOOD - all pixels having a common
 *                                      vertex with the current
 *                                      (8-nhood in 2-D, 26-nhood in 3-D)
 *   \param progress    If desired a progress reporter can be attached.
 *      All output will be redirected to the progress reporter and the
 *      progress will be updated regularly
 */
/*======================================================================*/
  template<typename Type, typename IndexT, int Dim>
  void
  extractLocalMaxima(blitz::Array<Type,Dim> const &data,
                     std::list< blitz::TinyVector<IndexT,Dim> > &localMaxima,
                     Type const &minValue,
                     NHood nh = COMPLEX_NHOOD,
                     iRoCS::ProgressReporter *progress = NULL);

/*======================================================================*/
/*! 
 *   Extraction of local maxima of a blitz::Array
 *
 *   \param data        The blitz::Array to search maxima in
 *   \param localMaxima A vector of TinyVectors, the local Minima are
 *                      appended to
 *   \param nh          Neighborhood to compare to. Possible values are\n
 *                      SIMPLE_NHOOD  - only direct neighbors along the
 *                                      axes (4-nhood in 2-D)\n
 *                      COMPLEX_NHOOD - all pixels having a common
 *                                      vertex with the current
 *                                      (8-nhood in 2-D, 26-nhood in 3-D)
 *   \param progress    If desired a progress reporter can be attached.
 *      All output will be redirected to the progress reporter and the
 *      progress will be updated regularly
 */
/*======================================================================*/
  template<typename Type, typename IndexT, int Dim>
  void
  extractLocalMaxima(blitz::Array<Type,Dim> const &data,
                     std::vector< blitz::TinyVector<IndexT,Dim> > &localMaxima,
                     NHood nh = COMPLEX_NHOOD,
                     iRoCS::ProgressReporter *progress = NULL);

/*======================================================================*/
/*! 
 *   Extraction of local maxima of a blitz::Array
 *
 *   \param data        The blitz::Array to search maxima in
 *   \param localMaxima A vector of TinyVectors, the local Minima are
 *                      appended to
 *   \param minValue    Only local maxima with value above minValue will
 *                      be added to the list (optional, if not given all
 *                      local maxima are returned)
 *   \param nh          Neighborhood to compare to. Possible values are\n
 *                      SIMPLE_NHOOD  - only direct neighbors along the
 *                                      axes (4-nhood in 2-D)\n
 *                      COMPLEX_NHOOD - all pixels having a common
 *                                      vertex with the current
 *                                      (8-nhood in 2-D, 26-nhood in 3-D)
 *   \param progress    If desired a progress reporter can be attached.
 *      All output will be redirected to the progress reporter and the
 *      progress will be updated regularly
 */
/*======================================================================*/
  template<typename Type, typename IndexT, int Dim>
  void
  extractLocalMaxima(blitz::Array<Type,Dim> const &data,
                     std::vector< blitz::TinyVector<IndexT,Dim> > &localMaxima,
                     Type const &minValue,
                     NHood nh = COMPLEX_NHOOD,
                     iRoCS::ProgressReporter *progress = NULL);

}

#include "LocalMaximumExtraction.icc"

#endif
