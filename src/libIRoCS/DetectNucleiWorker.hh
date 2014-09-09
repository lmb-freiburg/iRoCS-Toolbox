#ifndef DETECTNUCLEIWORKER_HH
#define DETECTNUCLEIWORKER_HH

#include <libProgressReporter/ProgressReporter.hh>

#include <libArrayToolbox/Array.hh>
#include <libArrayToolbox/ATBNucleus.hh>

namespace iRoCS
{
  
  void detectNuclei(
      atb::Array<double,3> const &data, std::vector<atb::Nucleus> &nuclei,
      std::string const &modelFileName, ptrdiff_t memoryLimit,
      std::string const &cacheFileName = "", ProgressReporter *pr = NULL);

}

#endif
