/**************************************************************************
 **       Title: Interface for Separable filters
 **    $RCSfile:$
 **   $Revision:$ $Name:$
 **       $Date:$
 **   Copyright: GPL $Author:$
 ** Description:
 **
 **************************************************************************/

/*======================================================================*/
/*!
 *  \file SeparableFilter.hh
 *  \brief Filter Interface additions that apply to separable filters
 */
/*======================================================================*/

#ifndef ATBSEPARABLEFILTER_HH
#define ATBSEPARABLEFILTER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "Filter.hh"

namespace atb
{
  
/*======================================================================*/
/*!
 *  \class SeparableFilter SeparableFilter.hh "libArrayToolbox/SeparableFilter.hh"
 *  \brief The SeparableFilter class specifies methods a separable filter
 *    provides additionally to the filter methods defined in
 *    Filter.
 *
 *  \see Filter
 */
/*======================================================================*/
  template<typename DataT, int Dim, typename ResultT>
  class SeparableFilter : public Filter<DataT,Dim,ResultT>
  {
    
  public:
    
/*======================================================================*/
/*! 
 *   Constructor.
 *
 *   \param btType        Defines the border treatment of this filter.
 *     The following border treatments are available:
 *     \c ValueBT, \c RepeatBT, \c MirrorBT, \c CyclicBT, \c CropBT
 *   \param boundaryValue The value to use for out-of-Array positions if
 *     the btType is ValueBT
 */
/*======================================================================*/
    SeparableFilter(
        BoundaryTreatmentType btType = ValueBT,
        DataT const &boundaryValue = traits<DataT>::zero);

/*======================================================================*/
/*! 
 *   Destructor.
 */
/*======================================================================*/
    virtual ~SeparableFilter();

/*======================================================================*/
/*! 
 *   Apply the filter along the specified dimension.
 *
 *   \param data           The data Array to filter
 *   \param elementSizeUm  The data voxel extents in micrometers
 *   \param result         The filter result
 *   \param dim            The dimension along which to apply the filter
 *
 *   \exception RuntimeError If an error occurs during the filter operation
 *     an exception of this kind is thrown
 */
/*======================================================================*/
    virtual void applyAlongDim(
        blitz::Array<DataT,Dim> const &data,
        blitz::TinyVector<double,Dim> const &elementSizeUm,
        blitz::Array<ResultT,Dim> &result, int dim,
        iRoCS::ProgressReporter *pr = NULL) const = 0;

/*======================================================================*/
/*! 
 *   Apply the filter along the specified dimension.
 *
 *   \param data           The data Array to filter
 *   \param result         The filter result
 *   \param dim            The dimension along which to apply the filter
 *
 *   \exception RuntimeError If an error occurs during the filter operation
 *     an exception of this kind is thrown
 */
/*======================================================================*/
    virtual void applyAlongDim(
        Array<DataT,Dim> const &data,
        Array<ResultT,Dim> &result, int dim,
        iRoCS::ProgressReporter *pr = NULL) const;
    
  };

}

#include "SeparableFilter.icc"

#endif
