#include "Random.hh"

namespace atb
{

  double gaussRandomNumber(const double mean, const double stddev) 
  {
    double x1, x2, w;
    do 
    {
      x1 = 2.0 * static_cast<double>(rand()) / RAND_MAX - 1.0;
      x2 = 2.0 * static_cast<double>(rand()) / RAND_MAX - 1.0;
      w = x1 * x1 + x2 * x2;
    }
    while (w >= 1.0);
    w = std::sqrt(-2.0 * std::log(w) / std::log(2.0) / w);
    return (x1 * w * stddev) + mean;
  }

}
