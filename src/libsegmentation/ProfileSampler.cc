#include "ProfileSampler.hh"
#include "ProfileSampler-inl.hh"
//explicit instantiations
template class segmentation::ProfileSampler<float, 3>;
template class segmentation::ProfileSampler<double, 3>;
template class segmentation::ProfileSampler<float, 2>;
template class segmentation::ProfileSampler<double, 2>;