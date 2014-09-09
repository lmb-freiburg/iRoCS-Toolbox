#include <blitz/array.h>

namespace atb 
{

/*======================================================================*/
/*! 
 *   Compute a normally distributed random variable, with given mean and
 *   standard deviation
 *
 *   \param mean   Mean of the gaussian distribution to draw a sample of
 *   \param stddev Standard Deviation of the gaussian distribution to draw
 *                 a sample of
 *
 *   \return A sample point drawn from a normal distribution
 */
/*======================================================================*/
  double gaussRandomNumber(const double mean, const double stddev);

/*======================================================================*/
/*! 
 *   Draw random vector from a uniform distribution with given upper
 *   bound (lower bound is zero)
 *
 *   \param maxValue    The upper bound of the vector components
 *
 *   \return A random vector from a uniform distribution
 */
/*======================================================================*/
  template<int Dim>
  blitz::TinyVector<double,Dim>
  drawRandomVector(const blitz::TinyVector<double,Dim>& maxValue);

/*======================================================================*/
/*! 
 *   Draw a random vector from a gaussian random variable with given
 *   mean and covariance matrix
 *
 *   \param mean     Mean of the gaussian distribution
 *   \param cov      Covariance matrix of the gaussian distribution
 *
 *   \return A random vector drawn from the gaussian distribution
 */
/*======================================================================*/
  template<int Dim>
  blitz::TinyVector<double,Dim>
  drawRandomVector(const blitz::TinyVector<double,Dim>& mean,
                   const blitz::TinyMatrix<double,Dim,Dim>& cov);

}

#include "Random.icc"
