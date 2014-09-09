/**************************************************************************
 **       Title: Deriche Filter base function.
 **    $RCSfile:$
 **   $Revision:$ $Name:$
 **       $Date:$
 **   Copyright: GPL $Author:$
 ** Description:
 **
 **   This is a convenience function used both in DericheFilter and in
 **   atb::Array, to avoid a unrecoverable circular dependency.
 **
 **************************************************************************/

/*======================================================================*/
/*!
 *  \file atb/DericheFilter_base.hh
 *  \brief Implementation of the basic Deriche filter function approximating
 *    Gaussian filtering using recursion (IIR).
 */
/*======================================================================*/

#ifndef ATBDERICHEFILTER_BASE_HH
#define ATBDERICHEFILTER_BASE_HH

#include <libProgressReporter/ProgressReporter.hh>

#include "TypeTrais.hh"
#include "Interpolator.hh"

namespace atb
{
  
  template<typename DataT, int Dim>
  void filterDericheAlongDim(
      blitz::Array<DataT,Dim> const &data,
      blitz::TinyVector<double,Dim> const &elementSizeUm,
      blitz::Array<DataT,Dim> &filtered,
      double sigmaUm, int dim,
      BoundaryTreatmentType btType = ValueBT,
      DataT const &boundaryValue = traits<DataT>::zero,
      iRoCS::ProgressReporter *pr = NULL);

}

#include "DericheFilter_base.icc"

#endif
