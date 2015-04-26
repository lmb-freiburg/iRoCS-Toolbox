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

#ifndef HELPERS_INL_HH
#define HELPERS_INL_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <vector>
#include <map>
#include <list>
#include <set>
#include <blitz/array.h>

namespace segmentation {
  
  

/**
 * Builds a vector of blitz Arrays (profiles) for selected clusters
 * each element of the vector corresponds to the profiles of
 * a selected cluster
 * @param profiles: all profiles, as returned by ProfileSampler.
 *  Rows in the Array correspond to indiviual profiles
 * @param labels: a vector which associated a label [0, size() -1]
 * to each profile
 * @param selected_clusters: only labels in this set are processed
 *
 * */
template<typename T>
std::vector<blitz::Array<T, 2> > collectSelectedClusterProfiles(
  const blitz::Array<T, 2>& profiles,
  const std::vector< ptrdiff_t >& labels,
  const std::set< ptrdiff_t >& selected_clusters) {

  using std::vector;
  using std::list;
  using std::map;
  typedef list<blitz::Array<float, 1> > ClusterList;

  // restructure the profiles by cluster id
  // only consider selected clusters
  map<const ptrdiff_t, ClusterList > profiles_by_cluster;
  // build list of profiles for each cluster
  for (ptrdiff_t row = 0; row < profiles.extent(0); ++row) {
    ptrdiff_t label = labels[row];
    // if selected
    if (selected_clusters.find(label) != selected_clusters.end()) {
      ClusterList &cluster_list = profiles_by_cluster[label];
      blitz::Array<float, 1> profile = profiles(row, blitz::Range::all());
      cluster_list.push_back(profile);
    }
  }

  // now build blitz Array with profiles for each selected cluster
  std::vector<blitz::Array<T, 2> > selected_profiles(selected_clusters.size());
  ptrdiff_t num_samples = profiles.extent(1);
  int i = 0;
  for(std::set<ptrdiff_t>::const_iterator it = selected_clusters.begin();
      it != selected_clusters.end(); ++it, ++i){
    ptrdiff_t label = *it;
    ClusterList &cluster_list = profiles_by_cluster[label];
    blitz::Array<float, 2> &cluster_profiles = selected_profiles[i];
    cluster_profiles.resize(cluster_list.size(), num_samples);
    int row = 0;
    for (ClusterList::const_iterator it = cluster_list.begin();
         it != cluster_list.end(); ++it, ++row) {
      cluster_profiles(row, blitz::Range::all()) = *it;
    }
  }
  return selected_profiles;
}

}

#endif
