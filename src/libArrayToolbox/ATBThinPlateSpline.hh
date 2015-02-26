/**************************************************************************
**       Title: Thin plate spline interpolation
**    $RCSfile:$
**   $Revision:$ $Name:  $
**       $Date:$
**   Copyright: GPL $Author:$
** Description:
**
**************************************************************************/

#ifndef ATBTHINPLATESPLINE_HH
#define ATBTHINPLATESPLINE_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "TypeTraits.hh"
#include "ATBLinAlg.hh"

#include <vector>

namespace atb
{

/*======================================================================*/
/*! 
 *   Compute the TPS distance between point p1 on point p2. This function
 *   is the fundamental solution of the biharmonic equation for the according
 *   dimension. The resulting value is used to weight the influences of the
 *   control points on the current evaluation point.
 *
 *   \param p1  The first point
 *   \param p2  The second point
 *
 *   \return Sort of distance measure between the two points for TPS
 *           computation
 */
/*======================================================================*/
  template<int Dim>
  double tpsDistance(
      const blitz::TinyVector<double,Dim>& p1,
      const blitz::TinyVector<double,Dim>& p2);

/*======================================================================*/
/*! 
 *   Given two vectors of correspondending points of two datasets, compute
 *   the weights and offsets of a thin plate spline that transforms the
 *   source point set onto the target point set.
 *
 *   To evaluate the spline at some position the coefficient matrix has to
 *   be applied to a vector of the form
 *   \f$\mathbf{W} \cdot (U(\vec{s}_0, \vec{x}), ..., U(\vec{s}_n, \vec{x}), x_1, ... x_d, 1)^T\f$
 *
 *   Thus for evaluation the source points (which are in fact the spline
 *   control points) will be needed again!
 *
 *   The following is a test example of how to use the ATBThinplateSpline
 *   functions to compute the thin plate spline coefficients and use them for
 *   interpolation later on:
 *   \code
 #include <ArrayToolbox.hh>
 #include <ATBThinPlateSpline.hh>

 ...

 std::vector< blitz::TinyVector<double,3> > target;
 std::vector< blitz::TinyVector<double,3> > source;
 
 for (atb::BlitzIndexT z = 0; z < 3; ++z) 
 {
   for (atb::BlitzIndexT y = 0; y < 3; ++y)
   {
     for (atb::BlitzIndexT x = 0; x < 3; ++x)
     {
       source.push_back(blitz::TinyVector<double,3>(z, y, x));
       target.push_back(blitz::TinyVector<double,3>(z - 2, y + 1, x - 1));
     }
   }
 }
 
 blitz::Array<double,2> coeffs;
 ATB::computeTPSParameters(source, target, coeffs);
 std::cout << coeffs << std::endl;
 for (size_t i = 0; i < source.size(); ++i)
   std::cout << "TPS(" << source[i] << ") = " << ATB::evaluateTPS(source[i], source, coeffs) << "    should be " << target[i] << std::endl;
 *   \endcode
 *
 *   \param source The source point set (n d-D Points) to map onto the target
 *                 point set
 *   \param target The corresponding target point set (n d-D Points)
 *   \param coeffs The \f$(n + d + 1 \times 3)\f$ coefficient matrix
 */
/*======================================================================*/
  template<int Dim>
  void computeTPSParameters(
      const std::vector< blitz::TinyVector<double,Dim> >& source,
      const std::vector< blitz::TinyVector<double,Dim> >& target,
      blitz::Array<double,2>& coeffs);

/*======================================================================*/
/*! 
 *   Given a set of controlPoints and a coefficient matrix as computed
 *   with computeTPSParameters(), evaluate the thin plate spline at position x
 *
 *   To evaluate the spline at some position the coefficient matrix has to
 *   be applied to a vector of the form
 *   \f$\mathbf{W} \cdot (U(\vec{s}_0, \vec{x}), ..., U(\vec{s}_n, \vec{x}), x_1, ... x_d, 1)^T\f$
 *
 *   The following is a test example of how to use the ATBThinplateSpline
 *   functions to compute the thin plate spline coefficients and use them for
 *   interpolation later on:
 *   \code
 #include <ArrayToolbox.hh>
 #include <ATBThinPlateSpline.hh>

 ...

 std::vector< blitz::TinyVector<double,3> > target;
 std::vector< blitz::TinyVector<double,3> > source;
 
 for (atb::BlitzIndexT z = 0; z < 3; ++z) 
 {
   for (atb::BlitzIndexT y = 0; y < 3; ++y)
   {
     for (atb::BlitzIndexT x = 0; x < 3; ++x)
     {
       source.push_back(blitz::TinyVector<double,3>(z, y, x));
       target.push_back(blitz::TinyVector<double,3>(z - 2, y + 1, x - 1));
     }
   }
 }
 
 blitz::Array<double,2> coeffs;
 ATB::computeTPSParameters(source, target, coeffs);
 std::cout << coeffs << std::endl;
 for (size_t i = 0; i < source.size(); ++i)
   std::cout << "TPS(" << source[i] << ") = " << ATB::evaluateTPS(source[i], source, coeffs) << "    should be " << target[i] << std::endl;
 *   \endcode
 *
 *   \param x              (d-D point) Position to evaluate the spline at
 *   \param controlPoints  The control points (d-D) (corresponds to source in
 *                         computeTPSParameters())
 *   \param coeffs         The \f$(n + d + 1 \times 3)\f$ coefficient matrix
 *
 *   \return The position the point x is mapped to by the spline
 *           interpolator
 */
/*======================================================================*/
  template<int Dim>
  blitz::TinyVector<double,Dim>
  evaluateTPS(
      const blitz::TinyVector<double,Dim>& x,
      const std::vector< blitz::TinyVector<double,Dim> >& controlPoints,
      const blitz::Array<double,2>& coeffs);

}

#include "ATBThinPlateSpline.icc"

#endif
