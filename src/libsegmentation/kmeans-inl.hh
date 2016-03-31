/**************************************************************************
 *
 * Copyright (C) 2015 Margret Keuper, Thorsten Falk
 *
 *        Image Analysis Lab, University of Freiburg, Germany
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 **************************************************************************/

#ifndef LIBMARGRET_SRC_KMEANS_INL_HH
#define LIBMARGRET_SRC_KMEANS_INL_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "kmeans.hh"

#include <time.h>

#include <blitz/array.h>
#include <vector>
#include <set>

#include "segmentation_error.hh"

namespace segmentation
{

  template<class DataT>
  _KMeans<DataT>::Distance::~Distance()
  {}

  template<class DataT>
  double _KMeans<DataT>::EuclideanDistance::operator()(
      const PointT& p1, const PointT& p2) const
  {
    PointT diff(p1.shape());
    diff = p1 - p2;
    return blitz::sum(diff*diff);
  }

  template<class DataT>
  _KMeans<DataT>::_KMeans(unsigned int k) :
          k_(k), max_iterations_(50), max_restarts_(100),
          variance_threshold_(0),
          variance_iter_no_improvement_(10),
          all(blitz::Range::all())
  {
    distance_ = new EuclideanDistance();
  }


  template<class DataT>
  _KMeans<DataT>::~_KMeans()
  {
    delete distance_;
  }

  template<class DataT>
  double _KMeans<DataT>::cluster(const ArrayPointT &points,
                                 ArrayPointT &best_means,
                                 std::vector<int> &labels)
  {
    num_points_ = points.extent(0);
    point_dim_ = points.extent(1);
    // throw exception if number of data point is lower than k
    if (num_points_ < k_)
    {
      std::stringstream s;
      s << "KMeans: Number of data points " << num_points_
        << " is less than k=" << k_;
      throw LabellingError(s.str().c_str());
    }
    points_.reference(points);

    means_.resize(k_, point_dim_);
    labels_.resize(num_points_, 0);

    //srand(static_cast<unsigned int>(time(NULL)));
    // compute boundaries
    //boundaries(points_, lbound_, ubound_);

    // restart multiple times and return lowest variance result
    best_means.resize(k_, point_dim_);
    std::vector<int> &best_labels = labels;
    best_labels.resize(k_);
    double min_variance = std::numeric_limits<double>::max();
    double last_variance = min_variance;
    // iterate till variance threshold or maximum number of restarts is reached
    for (int i = 0; i < max_restarts_ && min_variance > variance_threshold_; ++i)
    {
      initializeMeans();
      iterateUntilConvergence();
      double variance = totalVariance();
      if (variance < min_variance)
      {
        min_variance = variance;
        best_means = means_;
        best_labels = labels_;
      }
      // stop if variance didn't drop for x iterations
      if (i % variance_iter_no_improvement_ == 0)
      {
        if (last_variance == min_variance)
        {
          break;
        }
        last_variance = min_variance;
      }
    }
    return min_variance;
  }

  template<class DataT>
  void _KMeans<DataT>::iterateUntilConvergence()
  {
    for (int i = 0; i < max_iterations_; ++i)
    {
      //stop if labels don't change
      if (!assignLabels()) break;
      updateMeans();
    }
  }


  template<class DataT>
  void _KMeans<DataT>::initializeMeans()
  {
    std::set<ptrdiff_t> sampled_indices;
    while (sampled_indices.size() < k_)
    {
      ptrdiff_t rand_i = static_cast<ptrdiff_t>(
          static_cast<double>(rand()) / (RAND_MAX - 1) *
          static_cast<double>(num_points_));
      sampled_indices.insert(rand_i);
    }
    int i = 0;
    for (std::set<ptrdiff_t>::const_iterator it = sampled_indices.begin();
         it != sampled_indices.end(); ++it, ++i)
        means_(i, all) = points_(*it, all);
  }

  template<class DataT>
  void _KMeans<DataT>::updateMeans()
  {
    means_ = 0.;
    blitz::Array<double, 1> tt(4);
    blitz::Array<DataT, 1> count_points(k_);
    count_points = 0;
    for (ptrdiff_t i_p = 0; i_p < num_points_; ++i_p)
    {
      ptrdiff_t mean_idx = labels_[i_p];
      means_(mean_idx, all) += points_(i_p, all);
      ++count_points(mean_idx);
    }
    blitz::firstIndex i;
    blitz::secondIndex j;
    means_ = means_(i, j) / count_points(i);
    // means_(i, all) /= i; //count_points(i);
  
  }

  template<class DataT>
  bool _KMeans<DataT>::assignLabels()
  {
    bool changed = false;
    for (ptrdiff_t point_idx = 0; point_idx < num_points_; ++point_idx)
    {
      const PointT& point = points_(point_idx, all);
      double min_dist = std::numeric_limits<DataT>::max();
      int old_label = labels_[point_idx];
      // find nearest mean
      for (ptrdiff_t mean_idx = 0; mean_idx < k_; ++mean_idx)
      {
        const PointT &mean = means_(mean_idx, all);
        double dist = (*distance_)(mean, point);
        if (dist < min_dist)
        {
          min_dist = dist;
          labels_[point_idx] = mean_idx;
        }
      }
      if (!changed && old_label != labels_[point_idx])
          changed = true;
    }
    return changed;
  }

  template<class DataT>
  double _KMeans<DataT>::totalVariance()
  {
    double variance = 0;
    for (ptrdiff_t i = 0; i < num_points_; ++i)
    {
      double dist = (*distance_)(points_(i, all), means_(labels_[i], all));
      variance += blitz::pow2(dist);
    }
    return variance / static_cast<double>(num_points_);
  }

  template<class DataT>
  double kmeans(const unsigned int k,
                const blitz::Array<DataT, 2 > &points,
                blitz::Array<DataT, 2 > &means,
                std::vector<int> &labels
                )
  {
    // shortcut for point
    _KMeans<DataT> kmeans(k);
    return kmeans.cluster(points, means, labels);
    //kmeans.cluster();

  }

}
#endif
