#ifndef LAYERASSIGNMENTWORKER_HH
#define LAYERASSIGNMENTWORKER_HH

#include <libProgressReporter/ProgressReporter.hh>

#include <libArrayToolbox/Array.hh>
#include <libArrayToolbox/ATBNucleus.hh>
#include <libArrayToolbox/iRoCS.hh>

namespace iRoCS
{

  void assignLayers(
      atb::Array<double,3> const &data, std::vector<atb::Nucleus> &nuclei,
      atb::IRoCS const &iRoCS, std::string const &modelFileName,
      std::string const &cacheFileName = "",
      bool updateMitoses = false, bool cacheCoordinates = false,
      bool forceFeatureComputation = false, ProgressReporter *pr = NULL);

}

#endif
