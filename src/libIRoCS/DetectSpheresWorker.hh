#ifndef DETECTSPHERESWORKER_HH
#define DETECTSPHERESWORKER_HH

#include <libProgressReporter/ProgressReporter.hh>

#include <libArrayToolbox/HoughTransform.hh>
#include <libArrayToolbox/ATBNucleus.hh>

namespace iRoCS
{

  void detectSpheres(
      atb::Array<float,3> const &data, std::vector<atb::Nucleus> &spheres,
      atb::Array<float,3> &response, atb::Array<float,3> &radiusUm,
      blitz::TinyVector<double,2> const &radiusRangeUm, double radiusStepUm,
      double preSmoothingSigmaUm, double postSmoothingSigmaUm,
      double minMagnitude = 0.0, bool invertGradients = false,
      double gamma = 1.0, ProgressReporter *pr = NULL);

}

#endif
