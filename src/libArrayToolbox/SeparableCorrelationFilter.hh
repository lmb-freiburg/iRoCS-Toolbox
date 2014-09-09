/**************************************************************************
 **       Title: Separable n-D correlation
 **    $RCSfile:$
 **   $Revision:$ $Name:$
 **       $Date:$
 **   Copyright: GPL $Author:$
 ** Description:
 **
 **************************************************************************/

/*======================================================================*/
/*!
 *  \file atb/SeparableCorrelationFilter.hh
 *  \brief Classes and functions for n-D separable correlation.
 */
/*======================================================================*/

#ifndef ATBSEPARABLECORRELATIONFILTER_HH
#define ATBSEPARABLECORRELATIONFILTER_HH

#include "SeparableFilter.hh"

namespace atb
{
  
/*======================================================================*/
/*!
 *  \class SeparableCorrelationFilter SeparableCorrelationFilter.hh "atb/SeparableCorrelationFilter.hh"
 *  \brief The SeparableCorrelationFilter class implements direct
 *    separable correlation.
 *
 *  Compared to the ATB::correlate() functions this class provides
 *  functions with minimum memory overhead. The filters are parallelized
 *  and memory traversal is near optimum to give reasonable speed without
 *  employing Fourier Transforms. So this class is especially useful if you
 *  have only limited memory but much processing power. The running time
 *  scales linearly with the kernel size, though, therefore for big kernels
 *  this class can be much slower than its Fourier counterparts.
 */
/*======================================================================*/
  template<typename DataT,int Dim>
  class SeparableCorrelationFilter :
        public SeparableFilter<DataT,Dim,DataT>
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
    SeparableCorrelationFilter(
        BoundaryTreatmentType btType = ValueBT,
        DataT const &boundaryValue = traits<DataT>::zero);

/*======================================================================*/
/*! 
 *   Constructor with kernel intialization.
 *
 *   \param kernels   Pointers to the filter kernels for all data dimensions.
 *     The vector may contain \c NULL pointers for dimensions the filter
 *     should not be applied to.
 *   \param btType        Defines the border treatment of this filter.
 *     The following border treatments are available:
 *     \c ValueBT, \c RepeatBT, \c MirrorBT, \c CyclicBT, \c CropBT
 *   \param boundaryValue The value to use for out-of-Array positions if
 *     the btType is ValueBT
 */
/*======================================================================*/
    SeparableCorrelationFilter(
        blitz::TinyVector<blitz::Array<DataT,1> *,Dim> const &kernels,
        BoundaryTreatmentType btType = ValueBT,
        DataT const &boundaryValue = traits<DataT>::zero);

/*======================================================================*/
/*! 
 *   Destructor.
 */
/*======================================================================*/
    ~SeparableCorrelationFilter();

/*======================================================================*/
/*! 
 *   Set the filtering kernel component for the specified dimension.
 *   You can pass the \c NULL pointer if you don't want to filter in the
 *   specified direction. The filter will create and manage a copy of the
 *   given kernel, and not work directly on the data pointer given!
 *
 *   \param kernel         A pointer to the blitz++ Array containing the kernel
 *     to apply along dimension dim, or the \c NULL pointer if data
 *     should not be filtered along dim.
 *   \param dim            The filtering dimension for this kernel
 */
/*======================================================================*/
    void setKernelForDim(blitz::Array<DataT,1> const *kernel, int dim);

/*======================================================================*/
/*! 
 *   Set the kernels for all data dimensions. You can pass \c NULL pointers
 *   if you don't want to filter along some dimensions. The filter will
 *   create and manage copies of the given kernels, and will not work
 *   directly on the data pointers given!
 *
 *   \param kernels   Pointers to the filter kernels for all data dimensions.
 *     The vector may contain \c NULL pointers for dimensions the filter
 *     should not be applied to.
 */
/*======================================================================*/
    void setKernels(
        blitz::TinyVector<blitz::Array<DataT,1> *,Dim> const &kernels);

/*======================================================================*/
/*! 
 *   Correlation of the data Array with the kernel along the specified
 *   dimension.
 *
 *   \param data          The data Array to correlate
 *   \param elementSizeUm The voxel extents in micrometers
 *   \param filtered      The correlation result
 *   \param dim           The dimension along which to correlate
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
 *   Correlate the given data Array with this filter.
 *
 *   \param data          The data Array to correlate
 *   \param elementSizeUm The voxel extents in micrometers
 *   \param filtered      The correlation result
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
 *   Correlate the data Array with the given kernels using the specified
 *   boundary treatment policy.
 *
 *   \param data           The data Array to correlate
 *   \param elementSizeUm  The voxel extents in micrometers
 *   \param filtered       The correlation result
 *   \param kernels        Pointers to the filter kernels for all data
 *     dimensions. The vector may contain \c NULL pointers for dimensions
 *     the filter should not be applied to.
 *   \param btType         The boundary treatment to apply
 *   \param boundaryValue  The out-of-Array value that should be used if
 *     btType == \c ValueBT
 */
/*======================================================================*/
    static void apply(
        blitz::Array<DataT,Dim> const &data,
        blitz::TinyVector<double,Dim> const &elementSizeUm,
        blitz::Array<DataT,Dim> &filtered,
        blitz::TinyVector<blitz::Array<DataT,1> *,Dim> const &kernels,
        BoundaryTreatmentType btType = ValueBT,
        DataT const &boundaryValue = traits<DataT>::zero,
        iRoCS::ProgressReporter *pr = NULL);
    
/*======================================================================*/
/*! 
 *   Correlate the data Array with the given kernels using the specified
 *   boundary treatment policy.
 *
 *   \param data           The data Array to correlate
 *   \param filtered       The correlation result
 *   \param kernels        Pointers to the filter kernels for all data
 *     dimensions. The vector may contain \c NULL pointers for dimensions
 *     the filter should not be applied to.
 *   \param btType         The boundary treatment to apply
 *   \param boundaryValue  The out-of-Array value that should be used if
 *     btType == \c ValueBT
 */
/*======================================================================*/
    static void apply(
        Array<DataT,Dim> const &data, Array<DataT,Dim> &filtered,
        blitz::TinyVector<blitz::Array<DataT,1> *,Dim> const &kernels,
        BoundaryTreatmentType btType = ValueBT,
        DataT const &boundaryValue = traits<DataT>::zero,
        iRoCS::ProgressReporter *pr = NULL);

  private:
    
    blitz::TinyVector<blitz::Array<DataT,1>*,Dim> _kernels;

  };

}

#include "SeparableCorrelationFilter.icc"

#endif
