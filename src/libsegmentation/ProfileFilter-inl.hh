#ifndef LIBSEGMENTATION_SRC_PROFILEFILTER_INL_HH
#define LIBSEGMENTATION_SRC_PROFILEFILTER_INL_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "NormalPDF.hh"

#include "ProfileFilter.hh"
#include "ProfileSampler.hh"

#ifdef HAVE_BLITZ_V9
#include <blitz/tinyvec-et.h>
#endif

namespace segmentation
{

  template<class T, int Dim>
  blitz::RectDomain<Dim>
  ProfileFilter<T,Dim>::operator()(
      ProfileSampler<T,Dim> const &sampler,
      std::vector<NormalPDF<T>*> const &pdfs,
      blitz::TinyVector<double,Dim> centerUm,
      blitz::Array<T,Dim> const &data,
      blitz::TinyVector<double,Dim> const &elSize,
      blitz::Array<T,Dim> &result,
      blitz::TinyVector<double,Dim> lbUm,
      blitz::TinyVector<double,Dim> const &ubUm,
      iRoCS::ProgressReporter *progress) const
  {
    blitz::TinyVector<ptrdiff_t,Dim> lbPx(lbUm / elSize);
    blitz::TinyVector<ptrdiff_t,Dim> ubPx(ubUm / elSize);
    
    for (int d = 0; d < Dim; ++d)
    {
      if (lbPx(d) < 0) lbPx(d) = 0;
      if (ubPx(d) < 0) ubPx(d) = 0;
      if (lbPx(d) >= data.extent(d)) lbPx(d) = data.extent(d) - 1;
      if (ubPx(d) >= data.extent(d)) ubPx(d) = data.extent(d) - 1;
    }
    blitz::RectDomain<Dim> roi(lbPx, ubPx);
    lbUm = blitz::TinyVector<double,Dim>(lbPx) * elSize;

    typename blitz::Array<T,Dim>::iterator itDst;
    blitz::TinyVector<ptrdiff_t,Dim> outShape(ubPx - lbPx + 1);
    result.resize(outShape);

    MahalanobisDistance<T> dist;
    T maxVal = -std::numeric_limits<T>::infinity();

    ptrdiff_t progressCnt = 0;
    ptrdiff_t progressUpdate = 0;
    if (progress != NULL)
        progressUpdate = result.size() /
            (progress->taskProgressMax() - progress->taskProgressMin());

#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (ptrdiff_t i = 0; i < result.size(); ++i)
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

      blitz::TinyVector<ptrdiff_t,Dim> srcPosPx;
      ptrdiff_t tmp = i;
      for (int d = Dim - 1; d >= 0; --d)
      {
        srcPosPx(d) = tmp % result.extent(d);
        tmp /= result.extent(d);
      }
      srcPosPx += lbPx;

      blitz::TinyVector<double,Dim> srcPosUm(srcPosPx);
      srcPosUm *= elSize;

      // distance to nearest pdf
      blitz::Array<T,1> profile(
          sampler.sample(data, elSize, centerUm, srcPosUm));

      T minDist = std::numeric_limits<T>::infinity();
      for (size_t j = 0; j < pdfs.size(); ++j)
      {
        T d = dist(*(pdfs[j]), profile);
        if (d < minDist) minDist = d;
      }
      
      if(minDist == std::numeric_limits<T>::infinity())
      {
        if (blitz::any(srcPosUm != centerUm))
        {
#ifdef _OPENMP
#pragma omp critical
#endif
          std::cerr << "ProfileFilter: WARNING: nan value detected and set to "
                    << "infinity at position" << srcPosUm << std::endl;
        }
        minDist = -1;
      }

      // filter response distance to nearest pdf
      result.data()[i] = minDist;
#ifdef _OPENMP
#pragma omp critical
#endif
      {
        if (minDist > maxVal) maxVal = minDist;
      }
    }
#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (ptrdiff_t i = 0; i < result.size(); ++i)
        result.data()[i] = (result.data()[i] == -1) ? maxVal : result.data()[i];

    std::cout << "profile filter max val " << maxVal << std::endl;

    // invert grayvalues and normalize
#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (ptrdiff_t i = 0; i < result.size(); ++i)
        result.data()[i] = static_cast<T>(1) - result.data()[i] / maxVal;

    return roi;
  }

}
#endif //LIBSEGMENTATION_SRC_PROFILEFILTER_INL_HH
