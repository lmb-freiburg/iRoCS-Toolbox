/**************************************************************************
**       Title: Thin plate spline interpolation
**    $RCSfile:$
**   $Revision:$ $Name:  $
**       $Date:$
**   Copyright: GPL $Author:$
** Description:
**
**************************************************************************/

#include "ATBThinPlateSpline.hh"

#if defined HAVE_CONFIG_H && !defined PACKAGE
#include <config.hh>
#endif

#ifdef HAVE_BLITZ_V9
#include <blitz/tinyvec-et.h>
#endif

namespace atb
{
  
  template<>
  double tpsDistance(
      const blitz::TinyVector<double,1>& p1,
      const blitz::TinyVector<double,1>& p2)
  {
    double d = std::abs(p1(0) - p2(0));
    return d * d * d;
  }

  template<>
  double tpsDistance(
      const blitz::TinyVector<double,2>& p1,
      const blitz::TinyVector<double,2>& p2)
  {
    double n = std::sqrt(blitz::dot(p1 - p2, p1 - p2));
    return n * std::log(n);
  }

  template<>
  double tpsDistance(
      const blitz::TinyVector<double,3>& p1,
      const blitz::TinyVector<double,3>& p2)
  {
    return std::sqrt(blitz::dot(p1 - p2, p1 - p2));
  }

}

