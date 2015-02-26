#ifndef LIBMARGRET_EDGE_FILTER_H
#define LIBMARGRET_EDGE_FILTER_H

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <blitz/array.h>
#include <vector>
#include <libProgressReporter/ProgressReporter.hh>

namespace segmentation
{

/*!
 * calculate gradient magnitude with non-maximum-supression
 * \param data: input dataset
 * \param elSize: sampling size in micrometers
 * \param result: gradient magnitude with non-maximum supression
 **/
  template<class DataT>
  void edgeFilter(
      blitz::Array<DataT,3> const &data,
      blitz::TinyVector<double,3> const &elSize,
      blitz::Array<DataT,3> &result,
      blitz::TinyVector<double,3> const &lbUm = 0.0,
      blitz::TinyVector<double,3> const &ubUm =
      (std::numeric_limits<double>::infinity()),
      ATB::ProgressReporter *progress = NULL);

  template<class DataT>
  blitz::Array<DataT,3> _calculateGradients(
      blitz::Array<DataT,3> const &data,
      blitz::TinyVector<double,3> const &elSize,
      int direction, double scaling,
      ATB::ProgressReporter *progress = NULL);

}

#include "EdgeFilter-inl.hh"

#endif

