#ifndef DETECTNUCLEIWORKER_HH
#define DETECTNUCLEIWORKER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

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
