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

#ifndef LIBMARGRET_SRC_EDGEFILTER_INL_HH
#define LIBMARGRET_SRC_EDGEFILTER_INL_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "EdgeFilter.hh"

#include <libArrayToolbox/GaussianFilter.hh>
#include <libArrayToolbox/CentralGradientFilter.hh>
#include <omp.h>
#include <list>

namespace segmentation
{
  
  template<class DataT>
  void edgeFilter(
      blitz::Array<DataT,3> const &data,
      blitz::TinyVector<double,3> const &elSize,
      blitz::Array<DataT,3> &result,
      blitz::TinyVector<double,3> const &lbUm,
      blitz::TinyVector<double,3> const &ubUm,
      iRoCS::ProgressReporter *progress)
  {
    // Cut out ROI
    blitz::TinyVector<ptrdiff_t,3> lb(lbUm / elSize);
    blitz::TinyVector<ptrdiff_t,3> ub(ubUm / elSize);
    for (int d = 0; d < 3; ++d)
    {
      if (lb(d) < 0) lb(d) = 0;
      if (ub(d) < 0) ub(d) = 0;
      if (lb(d) >= data.extent(d)) lb(d) = data.extent(d) - 1;
      if (ub(d) >= data.extent(d)) ub(d) = data.extent(d) - 1;
    }
    blitz::RectDomain<3> roi(lb, ub);
    blitz::Array<DataT,3> cropped(ub - lb + 1);
    cropped = data(roi);

    // Smooth ROI
    blitz::TinyVector<double,3> sigma(elSize(1));
    atb::GaussianFilter<DataT,3>::apply(
        cropped, elSize, cropped, sigma, atb::BlitzIndexT(0), atb::RepeatBT);

    // Compute gradient
    blitz::Array<blitz::TinyVector<DataT,3>,3> dCropped;
    atb::CentralGradientFilter<DataT,3>(
        cropped, elSize, dCropped,
        atb::CentralGradientFilter<DataT,3>::SecondOrder, atb::MirrorBT);
    blitz::Array<DataT,3> dCroppedMag(cropped.shape());

    // Split up into magnitude and direction
#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (ptrdiff_t i = 0; i < dCropped.size(); ++i)
    {
      dCroppedMag.data()[i] = static_cast<DataT>(
          std::sqrt(blitz::dot(dCropped.data()[i], dCropped.data()[i])));
      if (dCroppedMag.data()[i] != 0.0)
          dCropped.data()[i] /= dCroppedMag.data()[i];
    }

    // Non maximum suppression
    result.resize(data.shape());
    result = 0;

    ptrdiff_t progressCnt = 0;
    ptrdiff_t progressUpdate = 0;
    if (progress != NULL)
        progressUpdate = cropped.size() /
            (progress->taskProgressMax() - progress->taskProgressMin());

    atb::LinearInterpolator<DataT,3> ip(atb::RepeatBT);

#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (ptrdiff_t i = 0; i < cropped.size(); ++i)
    {
      
      if (progress != NULL)
      {
        if (progress->isAborted()) continue;
        if (progressCnt % progressUpdate == 0)
            progress->updateProgress(
                static_cast<int>(progressCnt / progressUpdate +
                                 progress->taskProgressMin()));
#ifdef _OPENMP
#pragma omp atomic
#endif
        ++progressCnt;
      }

      blitz::TinyVector<ptrdiff_t,3> p;
      ptrdiff_t tmp = i;
      for (int d = 2; d >= 0; --d)
      {
        p(d) = tmp % cropped.extent(d);
        tmp /= cropped.extent(d);
      }
      blitz::TinyVector<double,3> pl(
          blitz::TinyVector<double,3>(p) - dCropped(p));
      blitz::TinyVector<double,3> pu(
          blitz::TinyVector<double,3>(p) + dCropped(p));
      
      if (dCroppedMag.data()[i] > ip.get(dCroppedMag, pl) &&
          dCroppedMag.data()[i] > ip.get(dCroppedMag, pu))
          result(blitz::TinyVector<ptrdiff_t,3>(p + lb)) =
              dCroppedMag.data()[i];
    }
  }

  template<class DataT>
  blitz::Array<DataT, 3> _calculateGradients(
      blitz::Array<DataT, 3> const &data,
      blitz::TinyVector<double,3> const &elSize,
      int direction, double scaling,
      iRoCS::ProgressReporter *progress)
  {
    //mit Randbehandlung
    blitz::TinyVector<ptrdiff_t,3> dataShape(data.shape());

    DataT maxVal = blitz::max(data);
    DataT minVal = blitz::min(data);

    blitz::Array<DataT,3> edgeNormedIm(dataShape);

    if (maxVal != minVal)
        edgeNormedIm = (data - minVal) / (maxVal - minVal);

    blitz::Array<DataT,3> dx(dataShape);
    dx = 0;

    DataT* data_in = edgeNormedIm.data();
    DataT* data_out_x = dx.data();

    ptrdiff_t r2r_offs = dataShape(2);
    ptrdiff_t l2l_offs = dataShape(1) * dataShape(2);

    switch (direction)
    {
    case 0:
    {
      // compute gradX
      for (ptrdiff_t lev = 0; lev < dataShape(0); ++lev)
      {
        for (ptrdiff_t row = 0; row < dataShape(1); ++row)
        {
          ptrdiff_t offset = lev * l2l_offs + row * r2r_offs + 1;
          for (ptrdiff_t col = 1; col < dataShape(2) - 1; ++col)
          {
            data_out_x[offset] =
                static_cast<DataT>(
                    0.5 * (data_in[offset+1] - data_in[offset-1]) * scaling);
            ++offset;
          }
        }
      }
      for (ptrdiff_t lev = 0; lev < dataShape(0); ++lev)
      {
        for (ptrdiff_t row = 0; row < dataShape(1); ++row)
        {
          dx(lev, row, static_cast<ptrdiff_t>(0)) =
              dx(lev, row, static_cast<ptrdiff_t>(1));
          dx(lev, row, dataShape(2) - 1) = dx(lev, row, dataShape(2) - 2);
        }
      }
      return dx;
      break;
    }
    case 1: {
      // compute gradY
      for (ptrdiff_t lev = 0; lev < dataShape(0); ++lev)
      {
        for (ptrdiff_t row = 1; row < dataShape(1) - 1; ++row)
        {
          ptrdiff_t offset = lev * l2l_offs + row * r2r_offs;
          for (ptrdiff_t col = 0; col < dataShape(2); ++col)
          {
            data_out_x[offset] = static_cast<DataT>(
                0.5 * (data_in[offset+r2r_offs]
                       - data_in[offset-r2r_offs]) * scaling);
            ++offset;
          }
        }
      }
      for (ptrdiff_t lev = 0; lev < dataShape(0); ++lev)
      {
        for (ptrdiff_t col = 0; col < dataShape(2); ++col)
        {
          dx(lev, static_cast<ptrdiff_t>(0), col) =
              dx(lev, static_cast<ptrdiff_t>(1), col);
          dx(lev, dataShape(1) - 1, col) = dx(lev, dataShape(1) - 2, col);
        }
      }
      return dx;
      break;
    }
    case 2: {
      // compute gradZ
      for (ptrdiff_t lev = 1; lev < dataShape(0) - 1; ++lev)
      {
        for (ptrdiff_t row = 0; row < dataShape(1); ++row)
        {
          ptrdiff_t offset = lev * l2l_offs + row * r2r_offs;
          for (ptrdiff_t col = 0; col < dataShape(2); ++col)
          {
            data_out_x[offset] = static_cast<DataT>(
                0.5 * (data_in[offset+l2l_offs]
                       - data_in[offset-l2l_offs]) * scaling);
            ++offset;
          }
        }
      }
      for (ptrdiff_t row = 0; row < dataShape(1); ++row)
      {
        for (ptrdiff_t col = 0; col < dataShape(2); ++col)
        {
          dx(static_cast<ptrdiff_t>(0), row, col) =
              dx(static_cast<ptrdiff_t>(1), row, col);
          dx(dataShape(0) - 1, row, col) = dx(dataShape(0) - 2, row, col);
        }
      }
      return dx;
      break;
    }
    default:
      return data;
    }
  }
} //namespace

#endif //guard
