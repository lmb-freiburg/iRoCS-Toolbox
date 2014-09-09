#include "gvf-inl.hh"

// explicit template instanciations

namespace segmentation
{

  template
  void gradientVectorFlowSOR(
      blitz::Array<blitz::TinyVector<float, 3>, 3> &gradient,
      blitz::TinyVector<float,3> const &el_size_um, float mu, float nu,
      int max_iter, iRoCS::ProgressReporter *progress);
  
  template
  void gradientVectorFlowSOR(
      blitz::Array<blitz::TinyVector<double, 3>, 3> &gradient,
      blitz::TinyVector<double,3> const &el_size_um, double mu, double nu,
      int max_iter, iRoCS::ProgressReporter *progress);

  template
  void msGradientVectorFlow(
      blitz::Array<blitz::TinyVector<float, 3>, 3> &gradient,
      blitz::TinyVector<float,3> const &el_size_um, float mu, float hs,
      float hr, int max_iter, iRoCS::ProgressReporter *progress);
  
  template
  void msGradientVectorFlow(
      blitz::Array<blitz::TinyVector<double, 3>, 3> &gradient,
      blitz::TinyVector<double,3> const &el_size_um, double mu, double hs,
      double hr, int max_iter, iRoCS::ProgressReporter *progress);

}
