#ifndef EPIDERMISLABELLINGWORKER_HH
#define EPIDERMISLABELLINGWORKER_HH

#include <libProgressReporter/ProgressReporter.hh>

#include <libArrayToolbox/Array.hh>
#include <libArrayToolbox/ATBNucleus.hh>

namespace iRoCS
{

  void labelEpidermis(
      atb::Array<double,3> const &data, std::vector<atb::Nucleus> &nuclei,
      std::string const &modelFileName, std::string const &cacheFileName = "",
      bool forceFeatureComputation = false, ProgressReporter *pr = NULL);

}

#endif
