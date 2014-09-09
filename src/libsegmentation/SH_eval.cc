#include "SH_eval.hh"

namespace segmentation 
{

  std::complex<double> evaluateSH(
      FrequencyArray const &coeffs, double theta, double phi)
  {
    std::complex<double> res = 0.0;
    int bw = static_cast<int>(std::sqrt(static_cast<double>(coeffs.size())));
    for (int l = 0; l < bw; ++l)
    {
      for (int m = -l; m < 0; ++m)
      {
        res += coeffs(linearIndex(bw, l, m)) *
            ((-m % 2 == 0) ? 1.0 : -1.0) *
            gsl_sf_legendre_sphPlm(l, -m, std::cos(theta)) *
            std::exp(
                std::complex<double>(0.0, 1.0) * static_cast<double>(m) * phi);
      }
      for (int m = 0; m <= l; ++m)
      {
        res += coeffs(linearIndex(bw, l, m)) *
            gsl_sf_legendre_sphPlm(l, m, std::cos(theta)) *
            std::exp(
                std::complex<double>(0.0, 1.0) * static_cast<double>(m) * phi);
      }
    }
    return res;
  }

}
