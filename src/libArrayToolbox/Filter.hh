/**************************************************************************
 **       Title: General filter interface
 **    $RCSfile:$
 **   $Revision:$ $Name:$
 **       $Date:$
 **   Copyright: GPL $Author:$
 ** Description:
 **
 **   The interface all filters should obey. It mainly sets up the boundary
 **   treatment and provides an apply() method.
 **
 **************************************************************************/

/*======================================================================*/
/*!
 *  \file atb/Filter.hh
 *  \brief Base class (Interface) for all Filter implementations.
 */
/*======================================================================*/

#ifndef ATBFILTER_HH
#define ATBFILTER_HH

#include "Array.hh"
#include "BoundaryTreatment.hh"

#include <libProgressReporter/ProgressReporter.hh>

namespace atb
{

/*======================================================================*/
/*!
 *  \class Filter Filter.hh "atb/Filter.hh"
 *  \brief The Filter class is the base class for all filter implementations
 *
 *  It is responsible for handling the boundary treatment and provides
 *  apply() methods to filter different types of Arrays.
 */
/*======================================================================*/
  template<typename DataT, int Dim, typename ResultT>
  class Filter 
  {

  public:
    
/*======================================================================*/
/*! 
 *   Constructor.
 *
 *   \param bt             The boundary treatment this filter uses
 *   \param boundaryValue  If bt is ValueBT, this value will be used for
 *     out-of-Array access
 */
/*======================================================================*/ 
    Filter(BoundaryTreatmentType bt = ValueBT,
           DataT const &boundaryValue = traits<DataT>::zero);
    
/*======================================================================*/
/*! 
 *   Destructor.
 */
/*======================================================================*/
    virtual ~Filter();
    
/*======================================================================*/
/*! 
 *   Get a read-only reference to the boundary treatment object associated
 *   with this filter.
 *
 *   \return A reference to the associated BoundaryTreatment object
 */
/*======================================================================*/
    BoundaryTreatment<DataT,Dim> const &boundaryTreatment() const;

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
 *   Apply the filter to the given Array.
 *
 *   \param data          The blitz++ Array to apply the filter to
 *   \param elementSizeUm The element size of the Array
 *   \param result        The filtering result
 *   \param pr            If given progress will be reported to this
 *     ProgressReporter
 *
 *   \exception RuntimeError If an error occurs during the filter operation
 *     an exception of this kind is thrown
 */
/*======================================================================*/
    virtual void apply(
        blitz::Array<DataT,Dim> const &data,
        blitz::TinyVector<double,Dim> const &elementSizeUm,
        blitz::Array<ResultT,Dim> &result,
        iRoCS::ProgressReporter *pr = NULL) const = 0;
    
/*======================================================================*/
/*! 
 *   Apply the filter to the given Array.
 *
 *   \param data          The Array to apply the filter to
 *   \param result        The filtering result
 *   \param pr            If given progress will be reported to this
 *     ProgressReporter
 *
 *   \exception RuntimeError If an error occurs during the filter operation
 *     an exception of this kind is thrown
 */
/*======================================================================*/
    virtual void apply(
        Array<DataT,Dim> const &data, Array<ResultT,Dim> &result,
        iRoCS::ProgressReporter *pr = NULL) const;
    
  protected:
    
/*======================================================================*/
/*! 
 *   The boundary treatment object to use for out-of-Array access. This
 *   is set during Filter object creation and can be changed by calling the
 *   setBoundaryTreatment() method. It gets destroyed, when the filter is
 *   deleted. If you delete the BoundaryTreatment object in a subclass,
 *   you must set this pointer to the NULL pointer.
 */
/*======================================================================*/
    BoundaryTreatment<DataT,Dim> *p_bt;
    
  };

}

#include "Filter.icc"

#endif
