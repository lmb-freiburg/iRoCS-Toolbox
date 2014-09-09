#include "RuntimeError.hh"

#include <blitz/array.h>

namespace atb
{

  enum NormType { MINMAX, STDDEV, ZERO_MEAN_STDDEV };

/*======================================================================*/
/*! 
 *   Normalize a blitz::Array
 *
 *   \param data     The Array to normalize the gray values
 *   \param dataNorm The Array the normalized data will be stored to.
 *                   You may give the same Array for data and dataNorm!
 *   \param t        Normalization Type, one of:
 *                   MINMAX, STDDEV, ZERO_MEAN_STDDEV
 *
 *   \exception ArrayToolsError
 */
/*======================================================================*/
  template<typename ScalarT, int Dim>
  void
  normalize(blitz::Array<ScalarT,Dim> const &data,
            blitz::Array<ScalarT,Dim> &dataNorm,
            NormType t);

}

#include "Normalization.icc"
