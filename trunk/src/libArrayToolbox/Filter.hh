/**************************************************************************
 *
 * Copyright (C) 2015 Thorsten Falk
 *
 *        Image Analysis Lab, University of Freiburg, Germany
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 **************************************************************************/

/*======================================================================*/
/*!
 *  \file Filter.hh
 *  \brief Base class (Interface) for all Filter implementations.
 */
/*======================================================================*/

#ifndef ATBFILTER_HH
#define ATBFILTER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "Array.hh"
#include "BoundaryTreatment.hh"

#include <libProgressReporter/ProgressReporter.hh>

namespace atb
{

/*======================================================================*/
/*!
 *  \class Filter Filter.hh "libArrayToolbox/Filter.hh"
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
