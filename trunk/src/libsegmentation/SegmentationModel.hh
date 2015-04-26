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

#ifndef LIBSEGMENTATION_SEGMENTATION_MODEL_HH
#define LIBSEGMENTATION_SEGMENTATION_MODEL_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <string>
#include <vector>
namespace segmentation{
// forward declarations
template<typename T>
class MahalanobisDistance;

template<typename T> class NormalPDF;
template<typename T>
class SegmentationModel{
  /**
   * A segmentation model ist just a list of Normal PDFs and the parameters
   * that where used for sampling
   * */
public:
  ~SegmentationModel();
  void appendPdf(NormalPDF<T> *pdf);
  void clear();
  size_t size();

  /**
   * saves model as hdf5-file
   * @param filename: file path where to save model
   * */
  void load(const std::string &filename);
  /**
   * loads model from hdf5-file
   * @param filename:  path wher to save model
   * */
  void save(const std::string &filename);

  /// sampling distance in um
  T sampling_dist_um_;
  int num_samples_;
  std::vector<NormalPDF<T>*> pdfs_;
};

} //namespace segmentation
#endif //LIBSEGMENTATION_SEGMENTATION_MODEL_HH
