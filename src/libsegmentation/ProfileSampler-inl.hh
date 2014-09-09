#ifndef LIBSEGMENTATION_SRC_PROFILESAMPLER_INL_HH
#define LIBSEGMENTATION_SRC_PROFILESAMPLER_INL_HH

#include "ProfileSampler.hh"

#include <omp.h>

#include <libBlitzHdf5/FileIOWrapper.hh>
#include <libArrayToolbox/Interpolator.hh>

namespace segmentation
{

  template<class T, int Dim>
  ProfileSampler<T,Dim>::ProfileSampler(int numSamples, T samplingDistUm)
          : _numSamples(numSamples), _samplingDistUm(samplingDistUm)
  {}

  template<class T, int Dim>
  blitz::Array<T,1> ProfileSampler<T, Dim>::sample(
      blitz::Array<T,Dim> const &data,
      blitz::TinyVector<double,Dim> const &elSize,
      blitz::TinyVector<double,Dim> const &centerUm,
      blitz::TinyVector<double,Dim> const &positionUm) const
  {
    // sampling direction, from center to edge
    blitz::TinyVector<double,Dim> samplingStepUm(positionUm - centerUm);

    // scale with sampling dist to optain stampling step vector
    // sampling step points away from the center
    samplingStepUm *= _samplingDistUm /
        std::sqrt(blitz::dot(samplingStepUm, samplingStepUm));

    // half total sampling vector, used as offset to sample in both
    // directions around position_um staring from the point closest to the
    // center
    blitz::TinyVector<double,Dim> samplingOffsetUm(
        0.5 * samplingStepUm * _numSamples);
    blitz::TinyVector<double,Dim> samplingStartUm(
        positionUm - samplingOffsetUm);

    atb::LinearInterpolator<T,Dim> ip;
    blitz::Array<T, 1> edgeSamples(_numSamples);
    for (int i = 0; i < _numSamples; ++i)
    {
      blitz::TinyVector<double,Dim> samplePosUm(
          samplingStartUm + samplingStepUm * static_cast<double>(i));
      blitz::TinyVector<double,Dim> samplePosPx(samplePosUm / elSize);
      edgeSamples(i) = ip.get(data, samplePosPx);
    }

    // forward finite difference approximation of edge_samples
    blitz::Array<T,1> dEdge(_numSamples - 1);
    for (int i = 0; i < _numSamples - 1; ++i)
        dEdge(i) = edgeSamples(i + 1) - edgeSamples(i);

    // normalize vector
    T normalizer = blitz::max(blitz::abs(dEdge));

    // we don't like our results to be screwed up by inf values
    if(normalizer > 0) dEdge /= normalizer;

    return dEdge;
  }

  template<class T, int Dim>
  void ProfileSampler<T, Dim>::sample(
      blitz::Array<T,Dim> const &data,
      blitz::TinyVector<double,Dim> const &elSize,
      blitz::TinyVector<double,Dim> const &centerUm,
      std::vector< blitz::TinyVector<double,Dim> > const &candidates,
      blitz::Array<T,2> &profiles,
      iRoCS::ProgressReporter *progress) const
  {
    profiles.resize(candidates.size(), _numSamples - 1);

    ptrdiff_t progressCnt = 0;
    ptrdiff_t progressUpdate = 0;
    if (progress != NULL)
        progressUpdate = candidates.size() /
            (progress->taskProgressMax() - progress->taskProgressMin());

#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(candidates.size()); ++i)
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
      profiles(i, blitz::Range::all()) =
          sample(data, elSize, centerUm, candidates[i]);
    }
  }

  template<class T, int Dim>
  void ProfileSampler<T, Dim>::sample(
      blitz::Array<T,Dim> const &data,
      blitz::TinyVector<double,Dim> const &elSize,
      blitz::TinyVector<double,Dim> const &centerUm,
      blitz::Array<T,Dim> const &candidates, T threshold,
      blitz::Array<T,2> &profiles,
      std::vector< blitz::TinyVector<double,Dim> > &usedCandidates,
      blitz::TinyVector<double,Dim> const &lbUm,
      blitz::TinyVector<double,Dim> const &ubUm,
      iRoCS::ProgressReporter *progress) const
  {
    usedCandidates.clear();

    blitz::TinyVector<ptrdiff_t,Dim> lb(lbUm / elSize);
    blitz::TinyVector<ptrdiff_t,Dim> ub(ubUm / elSize);
    for (int d = 0; d < Dim; ++d)
    {
      if (lb(d) < 0) lb(d) = 0;
      if (ub(d) < 0) ub(d) = 0;
      if (lb(d) >= data.extent(d)) lb(d) = data.extent(d) - 1;
      if (ub(d) >= data.extent(d)) ub(d) = data.extent(d) - 1;
    }
    blitz::RectDomain<Dim> roi(lb, ub);

    typename blitz::Array<T, Dim>::const_iterator it;
    for (it = candidates(roi).begin(); it != candidates(roi).end(); ++it)
    {
      if (*it < threshold) continue;
      blitz::TinyVector<double,Dim> positionUm(it.position() + lb);
      positionUm *= elSize;
      usedCandidates.push_back(positionUm);
    }
    sample(data, elSize, centerUm, usedCandidates, profiles, progress);
  }

} //namespace
#endif //LIBSEGMENTATION_SRC_PROFILESAMPLER_INL_HH
