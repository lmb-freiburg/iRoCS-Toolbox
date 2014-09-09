/**************************************************************************
 **       Title: Median Filter
 **    $RCSfile:$
 **   $Revision:$ $Name:$
 **       $Date:$
 **   Copyright: GPL $Author:$
 ** Description:
 **
 **   The median filter implementation
 **
 **************************************************************************/

/*======================================================================*/
/*!
 *  \file MedianFilter.hh
 *  \brief Median filter implementation
 */
/*======================================================================*/

#ifndef ATBMEDIANFILTER_HH
#define ATBMEDIANFILTER_HH

#include "Filter.hh"

namespace atb
{

/*======================================================================*/
/*!
 *  \class MedianFilter MedianFilter.hh "ArrayToolbox/MedianFilter.hh"
 *  \brief The MedianFilter class implements the n-dimensional median filter.
 */
/*======================================================================*/
  template<typename DataT, int Dim>
  class MedianFilter : public Filter<DataT,Dim,DataT>
  {

  public:
    
    typedef DataT ResultT;

/*======================================================================*/
/*! 
 *   Constructor. In contrast to other filters the median filter always
 *   uses crop boundary treatment. Therefore any changes in the
 *   setting of the boundary treatment are bogus and do not change the
 *   behaviour of the filter.
 *
 *   \param extentsPx  The filter extents in pixels
 */
/*======================================================================*/ 
    MedianFilter(
        blitz::TinyVector<BlitzIndexT,Dim> const &extentsPx =
        blitz::TinyVector<BlitzIndexT,Dim>(BlitzIndexT(1)));
    
/*======================================================================*/
/*! 
 *   Destructor.
 */
/*======================================================================*/
    virtual ~MedianFilter();

/*======================================================================*/
/*! 
 *   Get the filter extents in pixels. 
 *
 *   \return The filter extents in pixels. 
 */
/*======================================================================*/
    blitz::TinyVector<BlitzIndexT,Dim> const &filterExtentsPx() const;

/*======================================================================*/
/*! 
 *   Set the filter extents in pixels. 
 *
 *   \param extentsPx The filter extents in pixels. 
 */
/*======================================================================*/
    void setFilterExtentsPx(
        blitz::TinyVector<BlitzIndexT,Dim> const &extentsPx);

/*======================================================================*/
/*! 
 *   Apply the filter to the given Array.
 *
 *   \param data          The blitz++ Array to apply the filter to
 *   \param elementSizeUm The element size of the Array
 *   \param result        The filtering result
 *
 *   \exception RuntimeError If an error occurs during the filter operation
 *     an exception of this kind is thrown
 */
/*======================================================================*/
    virtual void apply(
        blitz::Array<DataT,Dim> const &data,
        blitz::TinyVector<double,Dim> const &elementSizeUm,
        blitz::Array<ResultT,Dim> &result,
        iRoCS::ProgressReporter *pr = NULL) const;
    
    // Explicitly force the name mangler to also consider the base class
    // implementation
    using atb::Filter<DataT,Dim,ResultT>::apply;
    
/*======================================================================*/
/*! 
 *   Apply the filter to the given Array.
 *
 *   \param data           The blitz++ Array to apply the filter to
 *   \param filtered       The filtering result
 *   \param extentsPx      The filter extents in pixels
 *
 *   \exception RuntimeError If an error occurs during the filter operation
 *     an exception of this kind is thrown
 */
/*======================================================================*/
    static void apply(
        blitz::Array<DataT,Dim> const &data,
        blitz::Array<ResultT,Dim> &filtered,
        blitz::TinyVector<BlitzIndexT,Dim> const &extentsPx,
        iRoCS::ProgressReporter *pr = NULL);

  private:
    
    blitz::TinyVector<BlitzIndexT,Dim> _filterExtentsPx;
   
  };

}

#include "MedianFilter.icc"

#endif
