/**************************************************************************
**       Title: Mathematical Morphology functions
**    $RCSfile:$
**   $Revision:$ $Name:$
**       $Date:$
**   Copyright: GPL $Author:$
** Description:
**
**************************************************************************/

#ifndef ATBMORPHOLOGY_HH
#define ATBMORPHOLOGY_HH

#include "TypeTraits.hh"
#include "Neighborhood.hh"
#include "ATBBasicTree.hh"

#include <libProgressReporter/ProgressReporter.hh>

#include <blitz/array.h>

#include <map>

/*======================================================================*/
/*!
 *  \file ATBMorphology.hh
 *  \brief A collection of morphological operations on blitz++ Arrays
 */
/*======================================================================*/

namespace atb
{

  enum NHood { SIMPLE_NHOOD, COMPLEX_NHOOD };

/*======================================================================*/
/*! 
 *   Generate a spherical neighborhood for morphological operations.
 *   The reference point is the center of the sphere.
 *
 *   \param elementSizeUm  The element size of the Array to apply the
 *     filter to. The structuring element will be spherical wrt. the
 *     given element size
 *  \param radiusUm        The radius of the structuring element in
 *     micrometers
 *
 *   \return The points within the spherical neighborhood
 */
/*======================================================================*/ 
  template<int Dim>
  std::vector< blitz::TinyVector<ptrdiff_t,Dim> > sphericalStructuringElement(
      blitz::TinyVector<double,Dim> const &elementSizeUm, double radiusUm);
  
/*======================================================================*/
/*! 
 *   Morphological dilation.
 *
 *   Bright structures are extended using the maximum operation over the
 *   given structuring element.
 *
 *   \param data      The Array to apply the filter to
 *   \param result    The output Array the result will be written to. This
 *     can be the same Array as the input Array.
 *   \param strel     The structuring element
 *   \param progress  Progress of the filter will be reported to the given
 *     ProgressReporter.
 */
/*======================================================================*/
  template<typename DataT, int Dim>
  void dilate(
      blitz::Array<DataT,Dim> const &data,
      blitz::Array<DataT,Dim> &result,
      std::vector< blitz::TinyVector<ptrdiff_t,Dim> > const &strel,
      iRoCS::ProgressReporter *progress = NULL);
  
/*======================================================================*/
/*! 
 *   Morphological erosion.
 *
 *   Dark structures are extended using the minimum operation over the
 *   given structuring element.
 *
 *   \param data      The Array to apply the filter to
 *   \param result    The output Array the result will be written to. This
 *     can be the same Array as the input Array.
 *   \param strel     The structuring element
 *   \param progress  Progress of the filter will be reported to the given
 *     ProgressReporter.
 */
/*======================================================================*/
  template<typename DataT, int Dim>
  void erode(
      blitz::Array<DataT,Dim> const &data,
      blitz::Array<DataT,Dim> &result,
      std::vector< blitz::TinyVector<ptrdiff_t,Dim> > const &strel,
      iRoCS::ProgressReporter *progress = NULL);
  
/*======================================================================*/
/*! 
 *   Morphological opening.
 *
 *   Dilation after Erosion using the given structuring element.
 *
 *   \param data      The Array to apply the filter to
 *   \param result    The output Array the result will be written to. This
 *     can be the same Array as the input Array.
 *   \param strel     The structuring element
 *   \param progress  Progress of the filter will be reported to the given
 *     ProgressReporter.
 */
/*======================================================================*/
  template<typename DataT, int Dim>
  void open(
      blitz::Array<DataT,Dim> const &data,
      blitz::Array<DataT,Dim> &result,
      std::vector< blitz::TinyVector<ptrdiff_t,Dim> > const &strel,
      iRoCS::ProgressReporter *progress = NULL);
  
/*======================================================================*/
/*! 
 *   Morphological closing.
 *
 *   Erosion after Dilation using the given structuring element.
 *
 *   \param data      The Array to apply the filter to
 *   \param result    The output Array the result will be written to. This
 *     can be the same Array as the input Array.
 *   \param strel     The structuring element
 *   \param progress  Progress of the filter will be reported to the given
 *     ProgressReporter.
 */
/*======================================================================*/
  template<typename DataT, int Dim>
  void close(
      blitz::Array<DataT,Dim> const &data,
      blitz::Array<DataT,Dim> &result,
      std::vector< blitz::TinyVector<ptrdiff_t,Dim> > const &strel,
      iRoCS::ProgressReporter *progress = NULL);

/*======================================================================*/
/*! 
 *   Morphological top-hat filter.
 *
 *   Computes data - open(data). This is useful for background removal, when
 *   searching for small structures. The structure element should be a little
 *   larger, than the biggest object.
 *
 *   \param data      The Array to apply the filter to
 *   \param result    The output Array the result will be written to. This
 *     can be the same Array as the input Array.
 *   \param strel     The structuring element
 *   \param progress  Progress of the filter will be reported to the given
 *     ProgressReporter.
 */
/*======================================================================*/
  template<typename DataT, int Dim>
  void tophat(
      blitz::Array<DataT,Dim> const &data,
      blitz::TinyVector<double,Dim> const &elementSizeUm,
      blitz::Array<DataT,Dim> &result,
      double radiusUm,
      iRoCS::ProgressReporter *progress = NULL);

/*======================================================================*/
/*! 
 *   Morphological hole filling for gray value data.
 *
 *   Local minima in the input image are filled so that for all pixels there
 *   is a path through the image with non-increasing intensities.
 *
 *   \param data The dataset to apply the fill holes algorithm to
 *   \param result The result of the hole filling operation
 *   \param nh     Connectivity of adjacant elements (Neighborhood)<br />
 *                 \c Simple  - all elements that share hyperplanes<br />
 *                 \c Complex - all elements that share a common vertex
 *   \param progress  Progress of the filter will be reported to the given
 *     ProgressReporter.
 */
/*======================================================================*/
  template <typename DataT, int Dim>
  void fillHolesGray(
      blitz::Array<DataT,Dim> const &data, blitz::Array<DataT,Dim> &result,
      enum Neighborhood<Dim>::Type nh = Neighborhood<Dim>::Complex,
      iRoCS::ProgressReporter *pr = NULL);

/*======================================================================*/
/*! 
 *   Connected component labelling of the given binary Array
 *
 *   \param data   Binary Array to find connected components in
 *   \param labels Integer Array the labelled regions are returned in
 *   \param nh     Connectivity of adjacant elements (Neighborhood)<br />
 *                 \c SIMPLE_NHOOD  - all elements that share hyperplanes<br />
 *                 \c COMPLEX_NHOOD - all elements that share a common vertex
 *   \param progress  Progress of the filter will be reported to the given
 *     ProgressReporter.
 */
/*======================================================================*/
  template<int Dim>
  void
  connectedComponentLabelling(const blitz::Array<bool,Dim>& data,
                              blitz::Array<BlitzIndexT,Dim>& labels,
                              NHood nh = COMPLEX_NHOOD,
                              iRoCS::ProgressReporter *pr = NULL);

}

#include "ATBMorphology.icc"

#endif
