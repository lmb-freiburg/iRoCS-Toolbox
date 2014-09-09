/**************************************************************************
 **       Title: Array correlation using fast fourier transform
 **    $RCSfile:$
 **   $Revision:$ $Name:$
 **       $Date:$
 **   Copyright: GPL $Author:$
 ** Description:
 **
 **************************************************************************/

/*======================================================================*/
/*!
 *  \file atb/FastCorrelationFilter.hh
 *  \brief Fast n-D correlation using the Fast Fourier Transform (FFT)
 */
/*======================================================================*/

#ifndef ATBFASTCORRELATIONFILTER_HH
#define ATBFASTCORRELATIONFILTER_HH

#include "Filter.hh"

#include <libBlitzFFTW/BlitzFFTW.hh>

#include <omp.h>

namespace atb
{
  
/*======================================================================*/
/*!
 *  \class FastCorrelationFilter "atb/FastCorrelationFilter.hh"
 *  \brief The FastCorrelationFilter class provides n-D Array correlation
 *    using the Fast Fourier Transform.
 *
 *  Specifically it uses the BlitzFFTW library. For non-separable kernels
 *  this is the fastest possibility to correlate data Arrays. It uses lots
 *  of memory, though, because it has to hold padded versions of the data
 *  Array, the kernel and the corresponding fourier transforms, therefore
 *  a memory overhead of factor 4 is the minimum, factor 6 to 8 is common.
 *  If you are low on memory and have separable kernels you should use the
 *  SeparableCorrelationFilter class instead. For big kernels it is slower,
 *  but it is almost in-place and fully parallelized to give optimum
 *  performance.
 */
/*======================================================================*/
  template<typename DataT, int Dim>
  class FastCorrelationFilter : public Filter<DataT,Dim,DataT>
  {

  public:

/*======================================================================*/
/*! 
 *   Default Constructor.
 *
 *   \param bt             The boundary treatment this filter uses. CropBT
 *     is not allowed for the FastCorrelationFilter and will result in a
 *     RuntimeError when the filter is applied!
 *   \param boundaryValue  If bt is ValueBT, this value will be used for
 *     out-of-Array access
 */
/*======================================================================*/ 
    FastCorrelationFilter(
        BoundaryTreatmentType bt = ValueBT,
        DataT const &boundaryValue = traits<DataT>::zero);
  
/*======================================================================*/
/*! 
 *   Constructor also taking the correlation kernel.
 *
 *   \param kernel         The filter kernel. Only a pointer to the kernel
 *     is stored, so, make sure that the kernel memory location stays valid
 *     as long as you use this filter.
 *   \param bt             The boundary treatment this filter uses. CropBT
 *     is not allowed for the FastCorrelationFilter and will result in a
 *     RuntimeError when the filter is applied!
 *   \param boundaryValue  If bt is ValueBT, this value will be used for
 *     out-of-Array access
 */
/*======================================================================*/ 
    FastCorrelationFilter(
        blitz::Array<DataT,Dim> const &kernel,
        BoundaryTreatmentType bt = ValueBT,
        DataT const &boundaryValue = traits<DataT>::zero);
  
/*======================================================================*/
/*! 
 *   Destructor.
 */
/*======================================================================*/
    ~FastCorrelationFilter();
    
/*======================================================================*/
/*! 
 *   Set the correlation kernel
 *
 *   \param kernel The correlation kernel to apply
 */
/*======================================================================*/
    void setKernel(blitz::Array<DataT,Dim> const &kernel);

/*======================================================================*/
/*! 
 *   Get the correlation kernel
 *
 *   \return The currently set correlation kernel
 */
/*======================================================================*/
    blitz::Array<DataT,Dim> const &kernel() const;

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
    void apply(
        blitz::Array<DataT,Dim> const &data,
        blitz::TinyVector<double,Dim> const &elementSizeUm,
        blitz::Array<DataT,Dim> &result,
        iRoCS::ProgressReporter *pr = NULL) const;
    
    // Explicitly force the name mangler to also consider the base class
    // implementation
    using atb::Filter<DataT,Dim,DataT>::apply;

/*======================================================================*/
/*! 
 *   Correlate the data Array with the given kernel using the psecified
 *   boundary treatment.
 *
 *   \param data           The blitz++ Array to apply the filter to
 *   \param elementSizeUm  The element size of the Array
 *   \param result         The filtering result
 *   \param kernel         The filter kernel
 *   \param btType         The boundary treatment to apply
 *   \param boundaryValue  The out-of-Array value that should be used if
 *     btType == \c ValueBT
 *
 *   \exception RuntimeError If an error occurs during the filter operation
 *     an exception of this kind is thrown
 */
/*======================================================================*/
    static void apply(
        blitz::Array<DataT,Dim> const &data,
        blitz::TinyVector<double,Dim> const &elementSizeUm,
        blitz::Array<DataT,Dim> &result,
        blitz::Array<DataT,Dim> const &kernel,
        BoundaryTreatmentType btType = ValueBT,
        DataT const &boundaryValue = traits<DataT>::zero,
        iRoCS::ProgressReporter *pr = NULL);
    
/*======================================================================*/
/*! 
 *   Correlate the data Array with the given kernel using the psecified
 *   boundary treatment.
 *
 *   \param data           The Array to apply the filter to
 *   \param result         The filtering result
 *   \param kernel         The filter kernel
 *   \param btType         The boundary treatment to apply
 *   \param boundaryValue  The out-of-Array value that should be used if
 *     btType == \c ValueBT
 *
 *   \exception RuntimeError If an error occurs during the filter operation
 *     an exception of this kind is thrown
 */
/*======================================================================*/
    static void apply(
        Array<DataT,Dim> const &data, Array<DataT,Dim> &result,
        blitz::Array<DataT,Dim> const &kernel,
        BoundaryTreatmentType btType = ValueBT,
        DataT const &boundaryValue = traits<DataT>::zero,
        iRoCS::ProgressReporter *pr = NULL);

  private:
  
    blitz::Array<DataT,Dim> const *p_kernel;
    mutable blitz::Array<std::complex<DataT>,Dim> _kernelFFTCache;

  };

}

#include "FastCorrelationFilter.icc"

#endif
