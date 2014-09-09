#ifndef LIBMARGRET_SRC_KMEANS_HH
#define LIBMARGRET_SRC_KMEANS_HH
#include <vector>
#include <blitz/array.h>

namespace segmentation
{


template<class DataT>
class _KMeans
{
  typedef typename blitz::Array<DataT, 1> PointT;
  typedef typename blitz::Array<DataT, 2> ArrayPointT;
public:
  struct Distance
  {
    virtual ~Distance();
    virtual double operator()(const PointT &p1, const PointT &p2) const = 0;
  };
  struct EuclideanDistance : public Distance
  {
    virtual double operator()(const PointT &p1, const PointT &p2) const;
  };

  _KMeans(const unsigned int k, const Distance &distance);
  _KMeans(const unsigned int k);
  virtual ~_KMeans();
  double cluster(const ArrayPointT &points,
                 ArrayPointT &means,
                 std::vector<ptrdiff_t> &labels);

  /*!
   * set maximum number of restarts to perform for variance minimization
   * \param max_restarts maximum number of restarts to perform
  */
  void setMaxRestarts(int max_restarts)
  {
    max_restarts_ = max_restarts;
  };
  /*!
   * set maximum number of iterations
   * \param max_iterations maximum number of iterations
  */
  void setMaxIterations(int max_iterations)
  {
    max_iterations_ = max_iterations;
  };
  /*!
   * Set variance threshold. No further restart is performed if total variance
   * drops below this threshold
  */
  void setMaxIterations(double threshold)
  {
    variance_threshold_ = threshold;
  };
  /*!
   * stop restarts if variance didn't drop for iter iterations
  */
  void setRestartStopThreshold(int iter)
  {
    variance_iter_no_improvement_ = iter;
  };
private:
  /*===========================================================================*/
  /*!
   * Helper function to randomly initialize the vector of cluster centers
   * \param lbound  lower bound
   * \param ubound  upper bound
   * \param points  the center points to initialize
   *
   * \exception MargretError
   */
  /*===========================================================================*/
  void iterateUntilConvergence();
  void initializeMeans();
  bool assignLabels();
  void updateMeans();
  double totalVariance();

  unsigned int k_;
  ptrdiff_t num_points_;
  ptrdiff_t point_dim_;
  /// maximum number of iterations
  int max_iterations_;
  /// maximum number of restarts (random reinitializations of means)
  int max_restarts_;
  double variance_threshold_;
  /// stop restarts if variance didn't drop in last x iterations
  int variance_iter_no_improvement_;
  const Distance* distance_;
  ArrayPointT points_;
  ArrayPointT means_;
  std::vector<ptrdiff_t> labels_;
  blitz::Range all;
  
  //upper and lower boundary of points
  //PointT ubound_;
  //PointT lbound_;


};

#define KMeans _KMeans


template<class DataT, int Dim>
double kmeans(const unsigned int k,
              const blitz::Array<DataT, 2 > &points,
              blitz::Array<DataT, 2 > &means,
              std::vector<ptrdiff_t> &labels
             );
}

//#include "kmeans-inl.hh"
#endif
