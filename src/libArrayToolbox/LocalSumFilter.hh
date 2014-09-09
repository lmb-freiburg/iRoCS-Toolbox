/**************************************************************************
 **       Title: Separable local sum Filter
 **    $RCSfile:$
 **   $Revision:$ $Name:$
 **       $Date:$
 **   Copyright: GPL $Author:$
 ** Description:
 **
 **************************************************************************/

/*======================================================================*/
/*!
 *  \file atb/LocalSumFilter.hh
 *  \brief Implementation of the local sum filter.
 *
 *    At each position the local sum in a specified box-shaped neighborhood
 *    is computed and returned.
 */
/*======================================================================*/

#ifndef ATBLOCALSUMFILTER_HH
#define ATBLOCALSUMFILTER_HH

#include "SeparableFilter.hh"

namespace atb
{
  
/*======================================================================*/
/*!
 *  \class LocalSumFilter LocalSumFilter.hh "atb/LocalSumFilter.hh"
 *  \brief The LocalSumFilter class is a box filter with a filter kernel
 *    containing only ones.
 *
 *  The filtering result can be divided by the kernel size to get a
 *  normalized box-filter response. It is implemented as an \f$O(n)\f$
 *  algorithm by exploiting the fact that for each kernel shift only the
 *  point entering the kernel has to be added and the point leaving the
 *  kernel has to be subtracted (for each direction).
 */
/*======================================================================*/
  template<typename DataT, int Dim>
  class LocalSumFilter : public SeparableFilter<DataT,Dim,DataT>
  {

  public:

/*======================================================================*/
/*! 
 *   Default Constructor.
 *
 *   \param btType        Defines the border treatment of this filter.
 *     The following border treatments are available:
 *     \c ValueBT, \c RepeatBT, \c MirrorBT, \c CyclicBT, \c CropBT
 *   \param boundaryValue The value to use for out-of-Array positions if
 *     the btType is ValueBT
 */
/*======================================================================*/
    LocalSumFilter(
        BoundaryTreatmentType btType = ValueBT,
        DataT const &boundaryValue = traits<DataT>::zero);

/*======================================================================*/
/*! 
 *   Constructor with kernel parameter intialization.
 *
 *   \param kernelShapePx  The box shape in pixels. If you pass values
 *     \f$\leq 0\f$ the filter will not be applied in the corresponding
 *     dimensions.
 *   \param btType        Defines the border treatment of this filter.
 *     The following border treatments are available:
 *     \c ValueBT, \c RepeatBT, \c MirrorBT, \c CyclicBT, \c CropBT
 *   \param boundaryValue The value to use for out-of-Array positions if
 *     the btType is ValueBT
 */
/*======================================================================*/
    LocalSumFilter(
        blitz::TinyVector<BlitzIndexT,Dim> const &kernelShapePx,
        BoundaryTreatmentType btType = ValueBT,
        DataT const &boundaryValue = traits<DataT>::zero);

/*======================================================================*/
/*! 
 *   Destructor.
 */
/*======================================================================*/
    ~LocalSumFilter();

/*======================================================================*/
/*! 
 *   Get the box width in the specified dimension.
 *
 *   \param dim The dimension to get the filter width for
 *
 *   \return The filterwidth along dimension dim in pixels
 */
/*======================================================================*/
    BlitzIndexT kernelSizePxInDim(int dim) const;
    
/*======================================================================*/
/*! 
 *   Set the box width in the specified dimension.
 *
 *   \param dim The dimension to set the filter width for
 *   \param kernelSizePx  The new width of the kernel along dimension dim
 *     in pixels.
 */
/*======================================================================*/
    void setKernelSizePxInDim(int dim, BlitzIndexT kernelSizePx);

/*======================================================================*/
/*! 
 *   Get the box kernel shape.
 *
 *   \return The kernel shape in pixels
 */
/*======================================================================*/
    blitz::TinyVector<BlitzIndexT,Dim> const &kernelShapePx() const;
    
/*======================================================================*/
/*! 
 *   Set the box kernel shape.
 *
 *   \param kernelShapePx  The new box shape in pixels. If you pass values
 *     \f$\leq 0\f$ the filter will not be applied in the corresponding
 *     dimensions.
 */
/*======================================================================*/
    void setKernelShapePx(
        blitz::TinyVector<BlitzIndexT,Dim> const &kernelShapePx);

/*======================================================================*/
/*! 
 *   Application of the local sum filter onto the data Array along the
 *   specified dimension.
 *
 *   \param data           The data Array to filter
 *   \param elementSizeUm  The voxel extents in micrometers
 *   \param filtered       The filter result
 *   \param dim            The dimension along which to apply the filter
 */
/*======================================================================*/
    void applyAlongDim(
        blitz::Array<DataT,Dim> const &data,
        blitz::TinyVector<double,Dim> const &elementSizeUm,
        blitz::Array<DataT,Dim> &filtered, int dim,
        iRoCS::ProgressReporter *pr = NULL) const;

    // Explicitly force the name mangler to also consider the base class
    // implementation
    using atb::SeparableFilter<DataT,Dim,DataT>::applyAlongDim;

/*======================================================================*/
/*! 
 *   Application of the local sum filter onto the data Array.
 *
 *   \param data           The data Array to filter
 *   \param elementSizeUm  The voxel extents in micrometers
 *   \param filtered       The filtering result
 */
/*======================================================================*/
    void apply(
        blitz::Array<DataT,Dim> const &data,
        blitz::TinyVector<double,Dim> const &elementSizeUm,
        blitz::Array<DataT,Dim> &filtered,
        iRoCS::ProgressReporter *pr = NULL) const;

    // Explicitly force the name mangler to also consider the base class
    // implementation
    using atb::Filter<DataT,Dim,DataT>::apply;

/*======================================================================*/
/*! 
 *   Application of a local sum filter with given kernel shape and
 *   boundary treatment policy onto the data Array.
 *
 *   \param data           The data Array to filter
 *   \param elementSizeUm  The voxel extents in micrometers
 *   \param filtered       The filtering result
 *   \param kernelShapePx  The box shape in pixels. If you pass values
 *     \f$\leq 0\f$ the filter will not be applied in the corresponding
 *     dimensions.
 *   \param btType         The boundary treatment to apply
 *   \param boundaryValue  The out-of-Array value that should be used if
 *     btType == \c ValueBT
 */
/*======================================================================*/
    static void apply(
        blitz::Array<DataT,Dim> const &data,
        blitz::TinyVector<double,Dim> const &elementSizeUm,
        blitz::Array<DataT,Dim> &filtered,
        blitz::TinyVector<BlitzIndexT,Dim> const &kernelShapePx,
        BoundaryTreatmentType btType = ValueBT,
        DataT const &boundaryValue = traits<DataT>::zero,
        iRoCS::ProgressReporter *pr = NULL);
    
/*======================================================================*/
/*! 
 *   Application of a local sum filter with given kernel shape and
 *   boundary treatment policy onto the data Array.
 *
 *   \param data           The data Array to filter
 *   \param filtered       The filtering result
 *   \param kernelShapePx  The box shape in pixels. If you pass values
 *     \f$\leq 0\f$ the filter will not be applied in the corresponding
 *     dimensions.
 *   \param btType         The boundary treatment to apply
 *   \param boundaryValue  The out-of-Array value that should be used if
 *     btType == \c ValueBT
 */
/*======================================================================*/
    static void apply(
        Array<DataT,Dim> const &data, Array<DataT,Dim> &filtered,
        blitz::TinyVector<BlitzIndexT,Dim> const &kernelShapePx,
        BoundaryTreatmentType btType = ValueBT,
        DataT const &boundaryValue = traits<DataT>::zero,
        iRoCS::ProgressReporter *pr = NULL);

  private:
    
    blitz::TinyVector<BlitzIndexT,Dim> _kernelShapePx;
    
  };
  
}

#include "LocalSumFilter.icc"

#endif
