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
 *  \file CentralGradientFilter.hh
 *  \brief The Implementation of the filter computing the gradient of
 *    the input data using central differences
 */
/*======================================================================*/

#ifndef ATBCENTRALGRADIENTFILTER_HH
#define ATBCENTRALGRADIENTFILTER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "SeparableFilter.hh"

namespace atb
{

/*======================================================================*/
/*!
 *  \class CentralGradientFilter CentralGradientFilter.hh "libArrayToolbox/CentralGradientFilter.hh"
 *  \brief The CentralGradientFilter class implements the SeparableFilter
 *    interface and provides gradient computation using central differences
 */
/*======================================================================*/
  template<typename DataT, int Dim>
  class CentralGradientFilter :
        public SeparableFilter< DataT,Dim,blitz::TinyVector<DataT,Dim> >
  {

  public:
    
    typedef blitz::TinyVector<DataT,Dim> ResultT;

    enum Accuracy { SecondOrder, FourthOrder };

/*======================================================================*/
/*! 
 *   Default Constructor. Creates a Gradient filter with second order
 *   accurate discrete gradient approimation using central differences.
 *
 *   \param bt             The boundary treatment this filter uses
 *   \param boundaryValue  If bt is ValueBT, this value will be used for
 *     out-of-Array access
 */
/*======================================================================*/ 
    CentralGradientFilter(
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
    CentralGradientFilter(
        Accuracy accuracy, BoundaryTreatmentType bt = ValueBT,
        DataT const &boundaryValue = traits<DataT>::zero);
    
/*======================================================================*/
/*! 
 *   Destructor.
 */
/*======================================================================*/
    virtual ~CentralGradientFilter();

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
 *   Apply the filter along the specified dimension.
 *
 *   \param data           The data Array to filter
 *   \param elementSizeUm  The data voxel extents in micrometers
 *   \param filtered       The filter result
 *   \param dim            The dimension along which to apply the filter
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
 *   \param dim            The dimension along which to apply the filter
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

#include "CentralGradientFilter.icc"

#endif
