/**************************************************************************
 *
 * Copyright (C) 2015 Kun Liu, Thorsten Falk
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

/*
 * lrootShapeAnalysis.cpp
 *
 *  Created on: Nov 4, 2010
 *      Author: liu
 */

#include "lrootShapeAnalysis.hh"

#include <cstring>

void volume(
    blitz::Array<int,3> const &label, blitz::Array<double,1> &volume,
    int maxLabel)
{
  if (maxLabel == -1) maxLabel = blitz::max(label);
  volume.resize(maxLabel);
  std::memset(volume.data(), 0, volume.size() * sizeof(double));
  int const *labels = label.data();
  double *volumes = volume.data();
  for (size_t i = 0; i < label.size(); ++i, ++labels)
      if (*labels > 0 && *labels <= maxLabel) volumes[*labels - 1] += 1.0;
}

void
centerAndVolume(
    blitz::Array<int,3> const &label,
    blitz::Array<blitz::TinyVector<double,3>,1> &centers,
    blitz::Array<double,1>& volumes, int maxLabel,
    blitz::TinyVector<double,3> const &elSize)
{
  if (maxLabel == -1) maxLabel = blitz::max(label);

  volumes.resize(maxLabel);
  std::memset(volumes.data(), 0, volumes.size() * sizeof(double));
  centers.resize(maxLabel);
  std::memset(centers.data(), 0,
              centers.size() * sizeof(blitz::TinyVector<double,3>));
  
  int const *labels = label.data();
  double *volume = volumes.data();
  blitz::TinyVector<double,3> *center = centers.data();
  for (int z = 0; z < label.extent(0); ++z)
  {
    for (int y = 0; y < label.extent(1); ++y)
    {
      for (int x = 0; x < label.extent(2); ++x, ++labels)
      {
        if (*labels <= 0 || *labels > maxLabel) continue;
        volume[*labels - 1] += 1.0;
        center[*labels - 1] += blitz::TinyVector<double,3>(
            z * elSize(0), y * elSize(1), x * elSize(2));
      }
    }
  }
  
  double elVolume = elSize(0) * elSize(1) * elSize(2);
  for (size_t i = 0; i < centers.size(); ++i, ++volume, ++center)
  {
    *center /= (*volume != 0.0) ? *volume : 1.0;
    *volume *= elVolume;
  }
}

void eraseMarkers(
    blitz::Array<int,3> const &L, blitz::Array<bool,3> &marker,
    blitz::Array<bool,1> const &flagTooSmall)
{
  marker.resize(L.shape());
#ifdef _OPENMP
#pragma omp parallel for
#endif
  for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(L.size()); ++i)
      marker.data()[i] = L.data()[i] > 0 && !flagTooSmall(L.data()[i] - 1);
}

void labelOnBorder(
    blitz::Array<int,3> const &L, blitz::Array<unsigned char,1> &validFlag)
{
  blitz::Range all = blitz::Range::all();

  blitz::Array<int, 2> slice(L(0, all, all));
#ifdef _OPENMP
#pragma omp parallel for
#endif
  for (int i = 0; i < slice.extent(0); i++)
  {
    for (int j = 0; j < slice.extent(1); j++)
    {
      if (slice(i, j) > 0)
      {
        validFlag(slice(i, j) - 1) = 0;
      }
    }
  }
  slice.reference(L(L.extent(0) - 1, all, all));

#ifdef _OPENMP
#pragma omp parallel for
#endif
  for (int i = 0; i < slice.extent(0); i++)
  {
    for (int j = 0; j < slice.extent(1); j++)
    {
      if (slice(i, j) > 0)
      {
        validFlag(slice(i, j) - 1) = 0;
      }
    }
  }
  slice.reference(L(all, 0, all));

#ifdef _OPENMP
#pragma omp parallel for
#endif
  for (int i = 0; i < slice.extent(0); i++)
  {
    for (int j = 0; j < slice.extent(1); j++)
    {
      if (slice(i, j) > 0)
      {
        validFlag(slice(i, j) - 1) = 0;
      }
    }
  }
  slice.reference(L(all, L.extent(1) - 1, all));

#ifdef _OPENMP
#pragma omp parallel for
#endif
  for (int i = 0; i < slice.extent(0); i++)
  {
    for (int j = 0; j < slice.extent(1); j++)
    {
      if (slice(i, j) > 0)
      {
        validFlag(slice(i, j) - 1) = 0;
      }
    }
  }
  slice.reference(L(all, all, 0));

#ifdef _OPENMP
#pragma omp parallel for
#endif
  for (int i = 0; i < slice.extent(0); i++)
  {
    for (int j = 0; j < slice.extent(1); j++)
    {
      if (slice(i, j) > 0)
      {
        validFlag(slice(i, j) - 1) = 0;
      }
    }
  }
  slice.reference(L(all, all, L.extent(2) - 1));

#ifdef _OPENMP
#pragma omp parallel for
#endif
  for (int i = 0; i < slice.extent(0); i++)
  {
    for (int j = 0; j < slice.extent(1); j++)
    {
      if (slice(i, j) > 0)
      {
        validFlag(slice(i, j) - 1) = 0;
      }
    }
  }
}

