#ifndef ATTACHIROCSWORKER_HH
#define ATTACHIROCSWORKER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <libProgressReporter/ProgressReporter.hh>
#include <libArrayToolbox/iRoCS.hh>

namespace iRoCS
{

  void attachIRoCS(
      atb::IRoCS &iRoCS, std::vector<atb::Nucleus> &nuclei,
      blitz::TinyVector<double,3> const &qcPositionUm, double kappa = 1.0,
      double lambda = 0.0, double mu = 0.0, double searchRadiusUm = 0.0,
      int nIterations = 1000, double tau = 0.1, ProgressReporter *pr = NULL);

}

#endif
