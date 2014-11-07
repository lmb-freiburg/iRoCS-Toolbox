/**************************************************************************
 **       Title: Central difference-based hessian filter
 **    $RCSfile:$
 **   $Revision:$ $Name:$
 **       $Date:$
 **   Copyright: GPL $Author:$
 ** Description:
 **
 **************************************************************************/

/*======================================================================*/
/*!
 *  \file atb/CentralHessianUTFilter.hh
 *  \brief The Implementation of the filter computing the hessian of
 *    the input data using central differences
 */
/*======================================================================*/

#ifndef ATBCENTRALHESSIANUTFILTER_HH
#define ATBCENTRALHESSIANUTFILTER_HH

#include "CentralGradientFilter.hh"

namespace atb
{

/*======================================================================*/
/*!
 *  \class CentralHessianUTFilter CentralHessianUTFilter.hh "libArrayToolbox/CentralHessianUTFilter.hh"
 *  \brief The CentralHessianUTFilter class implements the SeparableFilter
 *    interface and provides hessian computation using central differences.
 *
 *    The output is stored in upper triangular format to avoid extra memory
 *    consumption due to the redundant lower triangular values.
 */
/*======================================================================*/
  template<typename DataT, int Dim>
  class CentralHessianUTFilter :
        public SeparableFilter<
      DataT,Dim,blitz::TinyVector<DataT,Dim * (Dim + 1) / 2> >
  {

  public:
    
    typedef blitz::TinyVector<DataT,Dim * (Dim + 1) / 2> ResultT;

/*======================================================================*/
/*! 
 *   Default Constructor. Creates a Hessian filter with second order
 *   accurate discrete hessian approimation using central differences.
 *
 *   \param bt             The boundary treatment this filter uses
 *   \param boundaryValue  If bt is ValueBT, this value will be used for
 *     out-of-Array access
 */
/*======================================================================*/ 
    CentralHessianUTFilter(
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
    CentralHessianUTFilter(
        typename CentralGradientFilter<DataT,Dim>::Accuracy accuracy,
        BoundaryTreatmentType bt = ValueBT,
        DataT const &boundaryValue = traits<DataT>::zero);
    
/*======================================================================*/
/*! 
 *   Destructor.
 */
/*======================================================================*/
    virtual ~CentralHessianUTFilter();

/*======================================================================*/
/*! 
 *   Get the order of accuracy of this filter. Currently only snd and 4th
 *   order are implemented.
 *
 *   \return The accuracy of the discrete derivative approximation
 */
/*======================================================================*/
    typename CentralGradientFilter<DataT,Dim>::Accuracy accuracy() const;

/*======================================================================*/
/*! 
 *   Set the order of accuracy of this filter. Currently only snd and 4th
 *   order are implemented.
 *
 *   \param accuracy The new accuracy of the discrete derivative approximation
 */
/*======================================================================*/
    void setAccuracy(
        typename CentralGradientFilter<DataT,Dim>::Accuracy accuracy);

/*======================================================================*/
/*! 
 *   Compute the position in the triangular matrix for the given linear
 *   index.
 *
 *   \param dim  linear index into the local vectorial representation of the
 *     upper triangular matrix storing the Hessian.
 *
 *   \return The matrix position (row, column) corresponding to the given
 *     index
 */
/*======================================================================*/
    static blitz::TinyVector<int,2> indexToMatrixPosition(int dim);

/*======================================================================*/
/*! 
 *   Compute the linear index for the given matrix position.
 *
 *   \param pos The matrix position (row, column)
 *
 *   \return The linear index into the local vectorial representation of the
 *     upper triangular matrix storing the Hessian.
 */
/*======================================================================*/
    static int matrixPositionToIndex(blitz::TinyVector<int,2> const &pos);

/*======================================================================*/
/*! 
 *   Apply the filter along the specified dimension.
 *
 *   \param data           The data Array to filter
 *   \param elementSizeUm  The data voxel extents in micrometers
 *   \param filtered       The filter result
 *   \param dim            The component of the hessian to compute. The
 *     hessian is handled as an upper triangular matrix. The given dimension
 *     is the upper triangle index. E.g. for 2D: 0: d^2/dx^2, 1: d^2/dxdy,
 *     2: d^2/dy^2
 */
/*======================================================================*/
    virtual void applyAlongDim(
        blitz::Array<DataT,Dim> const &data,
        blitz::TinyVector<double,Dim> const &elementSizeUm,
        blitz::Array<DataT,Dim> &filtered, int dim,
        iRoCS::ProgressReporter *pr = NULL) const;

/*======================================================================*/
/*! 
 *   Apply the filter along the specified dimension.
 *
 *   \param data           The data Array to filter
 *   \param elementSizeUm  The data voxel extents in micrometers
 *   \param filtered       The filter result (in the dim'th vector component)
 *   \param dim            The component of the hessian to compute. The
 *     hessian is handled as an upper triangular matrix. The given dimension
 *     is the upper triangle index. E.g. for 2D: 0: d^2/dx^2, 1: d^2/dxdy,
 *     2: d^2/dy^2
 */
/*======================================================================*/
    virtual void applyAlongDim(
        blitz::Array<DataT,Dim> const &data,
        blitz::TinyVector<double,Dim> const &elementSizeUm,
        blitz::Array<ResultT,Dim> &filtered, int dim,
        iRoCS::ProgressReporter *pr = NULL) const;

    // Explicitly force the name mangler to also consider the base class
    // implementation
    using atb::SeparableFilter<DataT,Dim,ResultT>::applyAlongDim;

/*======================================================================*/
/*! 
 *   Apply the filter to the given Array.
 *
 *   \param data          The blitz++ Array to apply the filter to
 *   \param elementSizeUm The element size of the Array
 *   \param filtered      The filtering result
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
        typename CentralGradientFilter<DataT,Dim>::Accuracy accuracy,
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
        typename CentralGradientFilter<DataT,Dim>::Accuracy accuracy,
        BoundaryTreatmentType btType = ValueBT,
        DataT const &boundaryValue = traits<DataT>::zero,
        iRoCS::ProgressReporter *pr = NULL);

  private:
    
    typename CentralGradientFilter<DataT,Dim>::Accuracy _accuracy;

  };

}

#include "CentralHessianUTFilter.icc"

#endif