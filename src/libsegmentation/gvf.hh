#ifndef LIBSEGMENTATION_GVF_HH
#define LIBSEGMENTATION_GVF_HH

#include <blitz/array.h>

#include <libProgressReporter/ProgressReporter.hh>

namespace segmentation
{

/**
 * Solve Euler-Lagrange equation for gradient vector flow using
 * successive over-relaxation
 * \f[
 * 0 = \mu \Delta u_i - \|\nabla f\|^2 ( u_i - \frac{\partial f}{\partial i} )
 * \f]
 * u : output gradient vector flow
 * f : input vector field
 * */
  template<typename T>
  void gradientVectorFlowSOR(
      blitz::Array<blitz::TinyVector<T, 3>, 3> &gradient,
      blitz::TinyVector<T,3> const &el_size_um, T mu, T nu, int max_iter,
      iRoCS::ProgressReporter *progress = NULL);

  template<typename T>
  void msGradientVectorFlow(
      blitz::Array<blitz::TinyVector<T, 3>, 3> &gradient,
      blitz::TinyVector<T,3> const &el_size_um, T mu, T hs, T hr, int max_iter,
      iRoCS::ProgressReporter *progress = NULL);

}

#endif // LIBSEGMENTATION_GVF_HH
