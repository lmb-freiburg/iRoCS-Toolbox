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

#ifndef LIBSEGMENTATION_SRC_PROFILEFILTER_HH
#define LIBSEGMENTATION_SRC_PROFILEFILTER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <libProgressReporter/ProgressReporter.hh>

#include <vector>
#include <blitz/array.h>

namespace segmentation
{
  
  template<class T>
  class NormalPDF;

  template<class T, int Dim>
  class ProfileSampler;

  template<class T, int Dim>
  class ProfileFilter
  {

  public:

    ProfileFilter()
          {}

    /**
     * Filters input data. Calculates: 1. - normalized min distance to pdfs;
     * @param sampler: Profile sampler object
     * @param pdfs: list of NormalPDFs to calculate distance from
     * @param centerUm: position of cell center in micrometers
     * @param data: input data
     * @param elSize: element size in micrometers
     * @param result: output data
     * @param lb: Start sampling from position lb
     * @param ub: End sampling at position ub
     * @param progress: Progress is reported to the given reporter
     * */
    blitz::RectDomain<Dim> operator()(
        ProfileSampler<T,Dim> const &sampler,
        std::vector<NormalPDF<T>*> const &pdfs,
        blitz::TinyVector<double,Dim> centerUm,
        blitz::Array<T,Dim> const &data,
        blitz::TinyVector<double,Dim> const &elSize,
        blitz::Array<T,Dim> &result,
        blitz::TinyVector<double,Dim> lbUm = 0.0,
        blitz::TinyVector<double,Dim> const &ubUm =
        (std::numeric_limits<double>::infinity()),
        iRoCS::ProgressReporter *progress = NULL) const;

  };

} // namespace

#endif //LIBSEGMENTATION_SRC_PROFILEFILTER_HH
