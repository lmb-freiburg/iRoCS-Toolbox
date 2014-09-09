#ifndef LIBSEGMENTATION_SEGMENTATION_MODEL_HH
#define LIBSEGMENTATION_SEGMENTATION_MODEL_HH

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