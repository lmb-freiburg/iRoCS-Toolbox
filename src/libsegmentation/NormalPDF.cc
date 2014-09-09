#include "NormalPDF.hh"
#include "NormalPDF-inl.hh"

//explicit instantiations
template class segmentation::NormalPDF<double>;
template class segmentation::NormalPDF<float>;


template class segmentation::MahalanobisDistance<double>;
template class segmentation::MahalanobisDistance<float>;