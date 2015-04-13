/**************************************************************************
 *
 * Copyright (C) 2015 Thorsten Falk
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

#include "MitosisClassificationWorker.hh"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#endif 

#include "MitosisClassificationParameters.hh"

#include "iRoCSFeatures.hh"

#include <liblabelling_qt4/MultiChannelModel.hh>
#include <liblabelling_qt4/ChannelSpecsRenderer.hh>
#include <liblabelling_qt4/NucleusMarker.hh>

MitosisClassificationWorker::MitosisClassificationWorker(
    MitosisClassificationParameters const &parameters,
    iRoCS::ProgressReporter *progress,
    LabellingMainWidget* mainWidget,
    QWidget* parent) 
        : QThread(parent), _parameters(parameters), p_progress(progress),
          p_mainWidget(mainWidget), _sigmaMin(0.5), _sigmaMax(64.0),
          _sigmaStep(2.0), _bandMax(5)
{
  p_features = new iRoCS::Features(
      blitz::TinyVector<double,3>(1.0), p_progress);
}

MitosisClassificationWorker::~MitosisClassificationWorker()
{
  delete p_features;
}

void MitosisClassificationWorker::run()
{
  if (p_progress != NULL && !p_progress->updateProgressMessage(
          (tr("Loading normalization parameters from ") +
           _parameters.modelFileName().c_str()).toStdString())) return;
  p_features->loadNormalizationParameters(_parameters.modelFileName());

  if (p_progress != NULL && !p_progress->updateProgress(1)) return;

  int nFeatures = 0;
  for (double sigma = _sigmaMin; sigma <= _sigmaMax; sigma *= _sigmaStep)
      for (int laplace = 0; laplace <= _bandMax / 2; ++laplace)
          for (int band = 0; band <= _bandMax - 2 * laplace; ++band)
              nFeatures++;
  nFeatures += 4; // The hough features
    
  _testVectors.resize(_parameters.annotationChannel()->markers().size());

  for (size_t i = 0; i < _testVectors.size(); ++i)
      _testVectors[i].resize(nFeatures);
    
  if (p_progress != NULL && !p_progress->updateProgress(2)) return;

  // Check whether features need to be recomputed
  bool needsFeatureUpdate = false;
  for (ptrdiff_t j = 0; j < static_cast<ptrdiff_t>(_testVectors.size()); ++j)
  {
    if ((*_parameters.annotationChannel())[j]->needsFeatureUpdate())
    {
      needsFeatureUpdate = true;
      break;
    }
  }
  
  if (p_progress != NULL && !p_progress->updateProgress(3)) return;

  double progressPerChunk = 93.0;
  double progressLoadFeatures =
      ((needsFeatureUpdate) ? 0.8 : 0.1) * progressPerChunk;
  double progressStepPerFeatureLoad =
      progressLoadFeatures / static_cast<double>(nFeatures);
  double progressClassify = progressPerChunk - progressLoadFeatures;

  double progress = p_progress->progress();
    
  if (needsFeatureUpdate)
  {
    p_progress->updateProgressMessage(
        tr("Preparing feature vectors").toStdString());
    _parameters.annotationChannel()->resizeFeatures(nFeatures);

    if (p_progress != NULL && !p_progress->updateProgress(4)) return;

    // Compute SD features
    int feaIdx = 0;
    for (double sigma = _sigmaMin; sigma <= _sigmaMax; sigma *= _sigmaStep)
    {
      for (int laplace = 0; laplace <= _bandMax / 2; ++laplace)
      {
        int maxBand = _bandMax - 2 * laplace;
        for (int band = 0; band <= maxBand; ++band, ++feaIdx)
        {
          if (p_progress != NULL && p_progress->isAborted()) return;
          atb::Array<double,3> &fea = p_features->sdFeature(
              *_parameters.dataChannel()->data(),
              atb::SDMagFeatureIndex(sigma, laplace, band), maxBand,
              _parameters.cacheFileName());
          
          atb::LinearInterpolator<double,3> ip(atb::ValueBT);
#ifdef _OPENMP
#pragma omp parallel for
#endif
          for (ptrdiff_t j = 0;
               j < static_cast<ptrdiff_t>(_testVectors.size()); ++j)
          {
            Marker *marker = (*_parameters.annotationChannel())[j];
            if (!marker->needsFeatureUpdate() || p_progress->isAborted())
                continue;
            blitz::TinyVector<double,3> pos(
                marker->positionUm() / p_features->elementSizeUm());
            marker->feature(feaIdx) = ip.get(fea, pos);
          }
            
          progress += progressStepPerFeatureLoad;
          if (p_progress != NULL)
              p_progress->updateProgress(static_cast<int>(progress));
        }
        for (int band = 1; band <= _bandMax - 2 * laplace; ++band)
            p_features->deleteFeature(
                atb::SDMagFeatureIndex(sigma, laplace, band));
      }
      for (int laplace = 0; laplace <= _bandMax / 2; ++laplace)
          p_features->deleteFeature(
              atb::SDMagFeatureIndex(sigma, laplace, 0));
      p_features->deleteFeature(atb::SDMagFeatureIndex(sigma, 0, 0));
    }
      
    // Compute hough features
    for (int i = iRoCS::Features::PositiveMagnitude;
         i <= iRoCS::Features::NegativeRadius; ++i, ++feaIdx)
    {
      atb::Array<double,3> &fea = p_features->houghFeature(
          *_parameters.dataChannel()->data(), i, _parameters.cacheFileName());
        
      atb::LinearInterpolator<double,3> ip(atb::ValueBT);
#ifdef _OPENMP
#pragma omp parallel for
#endif
      for (ptrdiff_t j = 0; j < static_cast<ptrdiff_t>(_testVectors.size());
           ++j)
      {
        Marker *marker = (*_parameters.annotationChannel())[j];
        if (!marker->needsFeatureUpdate() || p_progress->isAborted())
            continue;
        blitz::TinyVector<double,3> pos(
            marker->positionUm() / p_features->elementSizeUm());
        marker->feature(feaIdx) = ip.get(fea, pos);
      }
        
      p_features->deleteFeature(i);
      progress += progressStepPerFeatureLoad;
        
      if (p_progress != NULL &&
          !p_progress->updateProgress(static_cast<int>(progress))) return;
    }
  }
  else
  {
    progress += progressLoadFeatures;
    p_progress->updateProgress(static_cast<int>(progress));
  }

#ifdef _OPENMP
#pragma omp parallel for
#endif
  for (ptrdiff_t j = 0; j < static_cast<ptrdiff_t>(_testVectors.size()); ++j)
  {
    for (int i = 0; i < static_cast<int>(nFeatures); ++i)
        (_testVectors[j])[i] =
            (*_parameters.annotationChannel())[j]->feature(i);
    (*_parameters.annotationChannel())[j]->setNeedsFeatureUpdate(false);
  }

  if (p_progress != NULL)
      p_progress->updateProgressMessage(
          tr("Normalizing features").toStdString());
  p_features->normalizeFeatures(_testVectors);
  progress += 1.0;
  if (p_progress != NULL &&
      !p_progress->updateProgress(static_cast<int>(progress))) return;

  if (p_progress != NULL) p_progress->setTaskProgressRange(
      static_cast<int>(
          5.0 + progressPerChunk + progressLoadFeatures),
      static_cast<int>(
          5.0 + progressPerChunk + progressLoadFeatures + progressClassify));

  p_features->classifyTwoClassSVM(
      _testVectors, _parameters.modelFileName());

  // Set mitosis flags according to classification
#ifdef _OPENMP
#pragma omp parallel for
#endif
  for (ptrdiff_t j = 0; j < static_cast<ptrdiff_t>(_testVectors.size()); ++j)
  {
    bool mitotic = static_cast<int>(_testVectors[j].getLabel()) > 0;
    NucleusMarker *marker =  static_cast<NucleusMarker*>(
        (*_parameters.annotationChannel())[j]);
    marker->setPredictedMitotic(mitotic);
    // Set mitotic if not manually set
    if (!marker->manual()) marker->setMitotic(mitotic);
  }
}
