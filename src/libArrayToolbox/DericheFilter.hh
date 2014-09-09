/**************************************************************************
 **       Title: Separable Deriche Filter
 **    $RCSfile:$
 **   $Revision:$ $Name:$
 **       $Date:$
 **   Copyright: GPL $Author:$
 ** Description:
 **
 **************************************************************************/

/*======================================================================*/
/*!
 *  \file atb/DericheFilter.hh
 *  \brief Implementation of the Deriche filter approximating Gaussian
 *    filtering using recursion (IIR).
 */
/*======================================================================*/

#ifndef ATBDERICHEFILTER_HH
#define ATBDERICHEFILTER_HH

#include "SeparableFilter.hh"
#include "DericheFilter_base.hh"

namespace atb
{
  
/*======================================================================*/
/*!
 *  \class DericheFilter DericheFilter.hh "atb/DericheFilter.hh"
 *  \brief The DericheFilter class implements the Deriche filter, which
 *     approximates a Gaussian filter using a recursive formulation.
 *
 *  The filter runs in \f$O(n)\f$, where \f$n\f$ is the number of Array
 *  elements compared to \f$O(n \times m)\f$ for a full Gaussian convolution,
 *  where \f$m\f$ is the kernel size. Beware of the fact, that the
 *  approximation is not good enough to give rotation invariant filter
 *  responses. 
 *
 *   Let \f$\alpha := \frac{5}{2\sqrt{\pi}\sigma}\f$ and \f$I\f$ be a
 *   line of an Array to be filtered, then the recursive filter is
 *   defined by \f$\tilde I_i = f_i + g_i\f$ where
 *
 *   \f[ f_i := \frac{\left( 1 - e^{\alpha} \right)^2}{1 + 2\alpha
 e^{-\alpha} - e^{-2\alpha}} \left( I_i + e^{-\alpha} \left( \alpha - 1
 \right) I_{i-1} \right) + 2e^{-\alpha} f_{i-1} - e^{-2\alpha} f_{i-2} \f]
 *   
 *   and
 *
 *   \f[ g_i := \frac{\left( 1 - e^{\alpha} \right)^2}{1 + 2\alpha
 e^{-\alpha} - e^{-2\alpha}} \left( e^{-\alpha} \left( \alpha + 1 \right)
 I_{i+1} - e^{-2\alpha} I_{i+2} \right) + 2e^{-\alpha} g_{i+1} - e^{-2\alpha}
 g_{i+2} \f]
 *
 */
/*======================================================================*/
  template<typename DataT, int Dim>
  class DericheFilter : public SeparableFilter<DataT,Dim,DataT>
  {

  public:

    typedef DataT ResultT;

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
    DericheFilter(
        BoundaryTreatmentType btType = ValueBT,
        DataT const &boundaryValue = traits<DataT>::zero);

/*======================================================================*/
/*! 
 *   Constructor.
 *
 *   \param sigmaUm  The standard deviations in micrometers. If
 *     you pass values \f$\leq 0\f$ the filter will not be applied in
 *     the corresponding dimensions.
 *   \param btType        Defines the border treatment of this filter.
 *     The following border treatments are available:
 *     \c ValueBT, \c RepeatBT, \c MirrorBT, \c CyclicBT, \c CropBT
 *   \param boundaryValue The value to use for out-of-Array positions if
 *     the btType is ValueBT
 */
/*======================================================================*/
    DericheFilter(
        blitz::TinyVector<double,Dim> const &sigmaUm,
        BoundaryTreatmentType btType = ValueBT,
        DataT const &boundaryValue = traits<DataT>::zero);

/*======================================================================*/
/*! 
 *   Destructor.
 */
/*======================================================================*/
    virtual ~DericheFilter();

/*======================================================================*/
/*! 
 *   Get the standard deviation of the Deriche filter along the given
 *   dimension.
 *
 *   \param dim The dimension to get the filter standard deviation for
 *
 *   \return The standard deviation of the filter in micrometers
 */
/*======================================================================*/
    double sigmaUmInDim(int dim) const;
    
/*======================================================================*/
/*! 
 *   Set the standard deviation of the Deriche filter along the given
 *   dimension.
 *
 *   \param dim      The dimension to set the filter standard deviation for
 *   \param sigmaUm  The new standard deviation along dimension dim. If
 *     you pass a value \f$\leq 0\f$ the filter will not be applied in
 *     that dimension.
 */
/*======================================================================*/
    void setSigmaUmInDim(int dim, double sigmaUm);

/*======================================================================*/
/*! 
 *   Get the standard deviations of the Deriche filter.
 *
 *   \return The standard deviations of the filter in micrometers
 */
/*======================================================================*/
    blitz::TinyVector<double,Dim> const &sigmaUm() const;
    
/*======================================================================*/
/*! 
 *   Set the standard deviations of the Deriche filter.
 *
 *   \param sigmaUm  The new standard deviations in micrometers. If
 *     you pass values \f$\leq 0\f$ the filter will not be applied in
 *     the corresponding dimensions.
 */
/*======================================================================*/
    void setSigmaUm(blitz::TinyVector<double,Dim> const &sigmaUm);

/*======================================================================*/
/*! 
 *   Application of the Deriche filter onto the data Array along the
 *   specified dimension.
 *
 *   \param data           The data Array to convolve
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
 *   Application of the Deriche filter onto the data Array using the
 *   standard deviations passed to the filter object.
 *
 *   \param data           The data Array to convolve
 *   \param elementSizeUm  The voxel extents in micrometers
 *   \param filtered       The convolution result
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
 *   Application of the Deriche filter onto the data Array using the
 *   standard deviations passed.
 *
 *   \param data           The data Array to convolve
 *   \param elementSizeUm  The voxel extents in micrometers
 *   \param filtered       The convolution result
 *   \param sigmaUm        The standard deviations per dimension in
 *     micrometers
 *   \param btType         The boundary treatment to apply
 *   \param boundaryValue  When using ValueBT this value is used for outside
 *     data access
 */
/*======================================================================*/
    static void DericheFilter<DataT,Dim>::apply(
        blitz::Array<DataT,Dim> const &data,
        blitz::TinyVector<double,Dim> const &elementSizeUm,
        blitz::Array<DataT,Dim> &filtered,
        blitz::TinyVector<double,Dim> const &sigmaUm,
        BoundaryTreatmentType btType = ValueBT,
        DataT const &boundaryValue = traits<DataT>::zero,
        iRoCS::ProgressReporter *pr = NULL)

/*======================================================================*/
/*! 
 *   Application of the Deriche filter onto the data Array using the
 *   standard deviations passed.
 *
 *   \param data           The data Array to convolve
 *   \param filtered       The convolution result
 *   \param sigmaUm        The standard deviations per dimension in
 *     micrometers
 *   \param btType         The boundary treatment to apply
 *   \param boundaryValue  When using ValueBT this value is used for outside
 *     data access
 */
/*======================================================================*/
    static void DericheFilter<DataT,Dim>::apply(
        Array<DataT,Dim> const &data,
        Array<DataT,Dim> &filtered,
        blitz::TinyVector<double,Dim> const &sigmaUm,
        BoundaryTreatmentType btType = ValueBT,
        DataT const &boundaryValue = traits<DataT>::zero,
        iRoCS::ProgressReporter *pr = NULL)

  private:
    
    blitz::TinyVector<double,Dim> _sigmaUm;
    
  };
  
}

#include "DericheFilter.icc"

#endif
