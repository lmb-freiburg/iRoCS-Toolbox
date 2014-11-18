/**************************************************************************
 **       Title: Separable n-D convolution
 **    $RCSfile:$
 **   $Revision:$ $Name:$
 **       $Date:$
 **   Copyright: GPL $Author:$
 ** Description:
 **
 **************************************************************************/

/*======================================================================*/
/*!
 *  \file SeparableConvolutionFilter.hh
 *  \brief Classes and functions for n-D separable convolution.
 */
/*======================================================================*/

#ifndef ATBSEPARABLECONVOLUTIONFILTER_HH
#define ATBSEPARABLECONVOLUTIONFILTER_HH

#include "SeparableCorrelationFilter.hh"

namespace atb
{
  
/*======================================================================*/
/*!
 *  \class SeparableConvolutionFilter SeparableConvolutionFilter.hh "libArrayToolbox/SeparableConvolutionFilter.hh"
 *  \brief The SeparableConvolutionFilter class implements direct
 *    separable convolution.
 *
 *  Compared to the ATB::convolve() functions this class provides
 *  functions with minimum memory overhead. The filters are parallelized
 *  and memory traversal is near optimum to give reasonable speed without
 *  employing Fourier Transforms. So this class is especially useful if you
 *  have only limited memory but much processing power. The running time
 *  scales linearly with the kernel size, though, therefore for big kernels
 *  this class can be much slower than its Fourier counterparts.
 */
/*======================================================================*/
  template<typename DataT,int Dim>
  class SeparableConvolutionFilter : public SeparableFilter<DataT,Dim,DataT>
  {

  public:
    
    typedef DataT ResultT;

/*======================================================================*/
/*! 
 *   Deafult Constructor.
 *
 *   \param btType        Defines the border treatment of this filter.
 *     The following border treatments are available:
 *     \c ValueBT, \c RepeatBT, \c MirrorBT, \c CyclicBT, \c CropBT
 *   \param boundaryValue The value to use for out-of-Array positions if
 *     the btType is ValueBT
 */
/*======================================================================*/
    SeparableConvolutionFilter(
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
    SeparableConvolutionFilter(
        blitz::TinyVector<blitz::Array<DataT,1> *,Dim> const &kernels,
        BoundaryTreatmentType btType = ValueBT,
        DataT const &boundaryValue = traits<DataT>::zero);

/*======================================================================*/
/*! 
 *   Destructor.
 */
/*======================================================================*/
    ~SeparableConvolutionFilter();

/*======================================================================*/
/*! 
 *   Change the boundary treatment type to the type specified.
 *
 *   \param bt             The boundary treatment this filter uses
 *   \param boundaryValue  If bt is ValueBT, this value will be used for
 *     out-of-Array access
 */
/*======================================================================*/
    void setBoundaryTreatment(
        BoundaryTreatmentType bt,
        DataT const &boundaryValue = traits<DataT>::zero);

/*======================================================================*/
/*! 
 *   Change the boundary treatment type to the type specified.
 *
 *   \param bt             The boundary treatment this filter uses
 */
/*======================================================================*/
    void setBoundaryTreatment(BoundaryTreatment<DataT,Dim> const &bt);

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
 *   Convolution of the data Array along the specified dimension
 *   using the filter kernel given through the kernel setters.
 *
 *   \param data          The data Array to convolve
 *   \param elementSizeUm The voxel extents in micrometers
 *   \param filtered      The convolution result
 *   \param dim           The dimension along which to convolve
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
 *   Convolve the given data Array with this filter.
 *
 *   \param data          The data Array to convolve
 *   \param elementSizeUm The voxel extents in micrometers
 *   \param filtered      The convolution result
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
 *   Convolve the data Array with the given kernels using the specified
 *   boundary treatment policy.
 *
 *   \param data           The data Array to convolve
 *   \param elementSizeUm  The voxel extents in micrometers
 *   \param filtered       The convolution result
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
 *   Convolve the data Array with the given kernels using the specified
 *   boundary treatment policy.
 *
 *   \param data           The data Array to convolve
 *   \param filtered       The convolution result
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
    
    SeparableCorrelationFilter<DataT,Dim> _filterReal;

  };

}

#include "SeparableConvolutionFilter.icc"

#endif
