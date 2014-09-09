#ifndef NORMALPDF_HH
#define NORMALPDF_HH

#include <blitz/array.h>

namespace segmentation {

/*===========================================================================*/
/*!
 * Encapsulates a normal pdf
 */
/*===========================================================================*/
template<typename T>
class NormalPDF {
public:
  /**
   * Estimate PDF from given set of samples
   * @param samples: A 2d array of samples, extent(0) is the number of samples
   *                 extend(1) the dimensionality of the samples
   */
  NormalPDF(const blitz::Array<T, 2> &samples);
  NormalPDF(const blitz::Array<T, 1> &mean,
            const blitz::Array<T, 2> &information);

  blitz::Array<T, 1> mean_;
  blitz::Array<T, 2> covariance_;
  blitz::Array<T, 2> information_matrix_;
  
  /**
   * Calculates information_matrix_
   * don't use information_matrix_ before calling this method
   * */
  void invertCovariance();
};

template<typename T>
class MahalanobisDistance {
public:
  MahalanobisDistance(){};
  T operator()(const NormalPDF<T>& pdf, const blitz::Array<T, 1>& sample);
};

}

#endif // NORMALPDF_HH
