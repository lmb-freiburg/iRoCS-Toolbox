/**************************************************************************
**       Title: Spherical Tensor Filter from Marco
**    $RCSfile: SphericalTensor.hh$
**   $Revision:$ $Name:$
**       $Date:$
**   Copyright: GPL $Author:$
** Description:
**
**   Fast computation of spherical tensors for rotation invariant feature
**   generation
**
**************************************************************************/

#ifndef SPHERICALTENSOR_HH
#define SPHERICALTENSOR_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <map>
#include <omp.h>

#include <libBlitzHdf5/FileIOWrapper.hh>

#include "Array.hh"

namespace atb
{
  
/*======================================================================*/
/*! 
 *   Data structure to index a feature structure consisting of multi-scale
 *   spherical derivative features
 */
/*======================================================================*/
  struct SDMagFeatureIndex
  {
    SDMagFeatureIndex(const double sigma, const int laplace, const int band)
            : s(sigma), l(laplace), b(band)
          {}
    ~SDMagFeatureIndex()
          {}
    double s;
    int l;
    int b;
    bool operator<(const SDMagFeatureIndex& rhs) const
          {
            if (s < rhs.s) return true;
            if (s == rhs.s && l < rhs.l) return true;
            if (s == rhs.s && l == rhs.l && b < rhs.b) return true;
            return false;
          }
  };

/*======================================================================*/
/*! 
 *   void STderivReal(double *in, BlitzIndexT sz[], BlitzIndexT L,
 *                    double *out, double factor = 1.0)
 *
 *   Remarks: * complex numbers are stored interleaved. 
 *	      * rank 0 tensor has also to be complex, i.e. of real
 *              dimension (2, sz[0],sz[1],sz[2])
 *	      * m-indices of spherical tensor are negative and reversed,
 *              i.e. -(L-1),-(L-2)...,0
 *
 *   \param in      volume of size (L*2 , sz[0], sz[1], sz[2])
 *   \param sz      size array of length 3
 *   \param L       rank of input, rank of spherical tensor is L-1
 *   \param out     preallocated output volume of size 
 *                  ((L+1)*2,  sz[0], sz[1], sz[2])
 *   \param factor  result is multiplied by this real factor
 */
/*======================================================================*/
  void STderivReal(
      double *in, BlitzIndexT sz[], BlitzIndexT L, double *out,
      double factor = 1.0);

/*======================================================================*/
/*! 
 *   Wrapper method for computing spherical tensorial derivative magnitude
 *   features (Marco)
 *
 *   \param data    The input Array to compute voxelwise features for
 *   \param out     The features sorted by the SDMagFeatureIndex
 *   \param sigma   The current feature scale (only for SDMagFeatureIndex)
 *   \param laplace The number of laplacians that have been applied before
 *                  (only for SDMagFeatureIndex)
 *   \param maxBand The maximum expansion band
 */
/*======================================================================*/
  template<typename DataT>
  void STderiv(const blitz::Array<DataT,3>& data,
               std::map<SDMagFeatureIndex,blitz::Array<DataT,3>*>& out,
               const double sigma, const int laplace,
               const BlitzIndexT maxBand);

/*======================================================================*/
/*! 
 *   Wrapper method for computing spherical tensorial derivative magnitude
 *   features (Marco)
 *
 *   \param data    The input Array to compute voxelwise features for
 *   \param out     The features sorted by the SDMagFeatureIndex
 *   \param sigma   The current feature scale (only for SDMagFeatureIndex)
 *   \param laplace The number of laplacians that have been applied before
 *                  (only for SDMagFeatureIndex)
 *   \param maxBand The maximum expansion band
 */
/*======================================================================*/
  template<typename DataT>
  void STderiv(Array<DataT,3> const &data,
               std::map<SDMagFeatureIndex,Array<DataT,3>*> &out,
               double sigma, int laplace, BlitzIndexT maxBand);

}

#include "SphericalTensor.icc"

#endif
