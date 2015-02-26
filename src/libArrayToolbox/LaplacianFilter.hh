/**************************************************************************
 **       Title: Forward-backward-difference-based Laplacian filter
 **    $RCSfile:$
 **   $Revision:$ $Name:$
 **       $Date:$
 **   Copyright: GPL $Author:$
 ** Description:
 **
 **************************************************************************/

/*======================================================================*/
/*!
 *  \file LaplacianFilter.hh
 *  \brief The Implementation of the filter computing the Laplacian of
 *    the input data using Forward-backward-differences
 */
/*======================================================================*/

#ifndef ATBLAPLACIANFILTER_HH
#define ATBLAPLACIANFILTER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "Filter.hh"

namespace atb
{

/*======================================================================*/
/*!
 *  \class LaplacianFilter LaplacianFilter.hh "libArrayToolbox/LaplacianFilter.hh"
 *  \brief The LaplacianFilter class implements the Filter interface and
 *    provides laplacian computation using forward-backward differences
 */
/*======================================================================*/
  template<typename DataT, int Dim>
  class LaplacianFilter :
        public Filter< DataT,Dim,DataT >
  {

  public:
    
    typedef DataT ResultT;

    enum Accuracy { SecondOrder, FourthOrder };

/*======================================================================*/
/*! 
 *   Default Constructor. Creates a Laplacian filter with second order
 *   accurate discrete gradient approimation using forward and backward
 *   differences.
 *
 *   \param bt             The boundary treatment this filter uses
 *   \param boundaryValue  If bt is ValueBT, this value will be used for
 *     out-of-Array access
 */
/*======================================================================*/ 
    LaplacianFilter(
        BoundaryTreatmentType bt = ValueBT,
        DataT const &boundaryValue = traits<DataT>::zero);
    
/*======================================================================*/
/*! 
 *   Constructor.
 *
 *   \param accuracy       The accuracy of the filter
 *   \param bt             The boundary treatment this filter uses
 *   \param boundaryValue  If bt is ValueBT, this value will be used for
 *     out-of-Array access
 */
/*======================================================================*/ 
    LaplacianFilter(
        Accuracy accuracy, BoundaryTreatmentType bt = ValueBT,
        DataT const &boundaryValue = traits<DataT>::zero);
    
/*======================================================================*/
/*! 
 *   Destructor.
 */
/*======================================================================*/
    virtual ~LaplacianFilter();

/*======================================================================*/
/*! 
 *   Get the order of accuracy of this filter. Currently only snd and 4th
 *   order are implemented.
 *
 *   \return The accuracy of the discrete derivative approximation
 */
/*======================================================================*/
    Accuracy accuracy() const;

/*======================================================================*/
/*! 
 *   Set the order of accuracy of this filter. Currently only snd and 4th
 *   order are implemented.
 *
 *   \param accuracy The new accuracy of the discrete derivative approximation
 */
/*======================================================================*/
    void setAccuracy(Accuracy accuracy);

/*======================================================================*/
/*! 
 *   Apply the filter to the given Array.
 *
 *   \param data          The blitz++ Array to apply the filter to
 *   \param elementSizeUm The element size of the Array
 *   \param filtered        The filtering result
 *
 *   \exception RuntimeError If an error occurs during the filter operation
 *     an exception of this kind is thrown
 */
/*======================================================================*/
    virtual void apply(
        blitz::Array<DataT,Dim> const &data,
        blitz::TinyVector<double,Dim> const &elementSizeUm,
        blitz::Array<ResultT,Dim> &filtered,
        iRoCS::ProgressReporter *pr = NULL) const;
    
    // Explicitly force the name mangler to also consider the base class
    // implementation
    using atb::Filter<DataT,Dim,ResultT>::apply;

/*======================================================================*/
/*! 
 *   Apply the filter to the given Array.
 *
 *   \param data           The blitz++ Array to apply the filter to
 *   \param elementSizeUm  The element size of the Array
 *   \param filtered       The filtering result
 *   \param accuracy       The filter accuracy order
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
        blitz::Array<ResultT,Dim> &filtered,
        Accuracy accuracy,
        BoundaryTreatmentType btType = ValueBT,
        DataT const &boundaryValue = traits<DataT>::zero,
        iRoCS::ProgressReporter *pr = NULL);

/*======================================================================*/
/*! 
 *   Apply the filter to the given Array.
 *
 *   \param data          The blitz++ Array to apply the filter to
 *   \param filtered      The filtering result
 *   \param accuracy      The filter accuracy order
 *   \param btType        The boundary treatment to apply
 *   \param boundaryValue If btType is Value, this value is assumed outside
 *     the Arrays
 *   \param btType         The boundary treatment to apply
 *   \param boundaryValue  The out-of-Array value that should be used if
 *     btType == \c ValueBT
 *
 *   \exception RuntimeError If an error occurs during the filter operation
 *     an exception of this kind is thrown
 */
/*======================================================================*/
    static void apply(
        Array<DataT,Dim> const &data, Array<ResultT,Dim> &filtered,
        Accuracy accuracy,
        BoundaryTreatmentType btType = ValueBT,
        DataT const &boundaryValue = traits<DataT>::zero,
        iRoCS::ProgressReporter *pr = NULL);

  private:
    
    Accuracy _accuracy;

  };

}

#include "LaplacianFilter.icc"

#endif
