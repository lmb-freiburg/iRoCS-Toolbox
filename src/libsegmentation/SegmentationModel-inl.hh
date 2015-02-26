#ifndef LIBSEGMENTATION_SRC_SEGMENTATION_MODEL_HH
#define LIBSEGMENTATION_SRC_SEGMENTATION_MODEL_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "SegmentationModel.hh"
#include "NormalPDF.hh"
#include <libBlitzHdf5/BlitzHdf5Light.hh>

namespace segmentation {


template<typename T>
void SegmentationModel<T>::appendPdf(NormalPDF< T >* pdf) {
  pdfs_.push_back(pdf);
}

template<typename T>
SegmentationModel<T>::~SegmentationModel() {
  for (size_t i = 0; i < pdfs_.size(); ++i)
    delete pdfs_[i];
}

template<typename T>
void SegmentationModel<T>::clear() {
  pdfs_.clear();
}

template<typename T>
size_t SegmentationModel<T>::size() {
  return pdfs_.size();
}

template<typename T>
void SegmentationModel<T>::load(const std::string &filename) {
  clear();
  BlitzH5File file(filename, BlitzH5File::ReadOnly);
  // sampling
  file.readAttribute(num_samples_, "num_samples", "/libsegmentation");
  file.readAttribute(sampling_dist_um_, "sampling_dist_um", "/libsegmentation");

  // pdfs
  size_t num_pdfs;
  file.readAttribute(num_pdfs, "num_pdfs", "/libsegmentation");
  pdfs_.resize(num_pdfs);
  for (size_t i = 0; i < num_pdfs; ++i) {
    std::stringstream pdf_prefix;
    pdf_prefix << "/libsegmentation/pdf_" << i;

    blitz::Array<T, 2> information;
    blitz::Array<T, 1> mean;
    file.readDataset(mean, pdf_prefix.str() + "/mean");
    file.readDataset(information, pdf_prefix.str() + "/information");
    pdfs_[i] = new segmentation::NormalPDF<T>(mean, information);
  }
}
template<typename T>
void SegmentationModel<T>::save(const std::string &filename) {
  BlitzH5File file(filename, BlitzH5File::Replace);

  // sampling
  file.writeAttribute(num_samples_, "num_samples", "/libsegmentation");
  file.writeAttribute(
      sampling_dist_um_, "sampling_dist_um", "/libsegmentation");
  // write pdfs
  file.writeAttribute(pdfs_.size(), "num_pdfs", "/libsegmentation");
  for (size_t i = 0; i < pdfs_.size(); ++i) {
    std::stringstream pdf_prefix;
    pdf_prefix << "/libsegmentation/pdf_" << i;
    const segmentation::NormalPDF<T> *pdf = pdfs_[i];
    file.writeDataset(pdf->mean_, pdf_prefix.str() +  "/mean");
    file.writeDataset(pdf->covariance_, pdf_prefix.str() +  "/information");
  }
}

}
#endif //LIBSEGMENTATION_SRC_SEGMENTATION_MODEL_HH
