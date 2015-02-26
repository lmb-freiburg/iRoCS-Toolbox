#ifndef SH_EVAL_HH
#define SH_EVAL_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "SH_tools.hh"

namespace segmentation 
{
  
  std::complex<double> evaluateSH(
      FrequencyArray const &coeffs, double theta, double phi);

}

#endif
