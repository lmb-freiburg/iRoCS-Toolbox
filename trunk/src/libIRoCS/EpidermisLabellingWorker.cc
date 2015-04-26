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

#include "EpidermisLabellingWorker.hh"
#include "iRoCSFeatures.hh"

namespace iRoCS
{

  void labelEpidermis(
      atb::Array<double,3> const &data, std::vector<atb::Nucleus> &nuclei,
      std::string const &modelFileName, std::string const &cacheFileName,
      bool forceFeatureComputation, ProgressReporter *pr)
  {
    double sigmaMin = 0.5;
    double sigmaMax = 64.0;
    double sigmaStep = 2.0;
    int bandMax = 5;

    Features features(blitz::TinyVector<double,3>(1.0), pr);

    if (pr != NULL && !pr->updateProgressMessage(
            "Loading normalization parameters from " + modelFileName)) return;
    try
    {
      features.loadNormalizationParameters(modelFileName);
    }
    catch (std::exception &e)
    {
      if (pr != NULL) pr->abortWithError(
          "Could not load feature normalization parameters from '" +
          modelFileName + ": " + e.what());
      return;
    }
    catch (...)
    {
      if (pr != NULL) pr->abortWithError(
          "Could not load feature normalization parameters from '" +
          modelFileName + ": unknown error");
      return;
    }

    if (pr != NULL && !pr->updateProgress(1)) return;

    int nFeatures = 0;
    for (double sigma = sigmaMin; sigma <= sigmaMax; sigma *= sigmaStep)
        for (int laplace = 0; laplace <= bandMax / 2; ++laplace)
            for (int band = 0; band <= bandMax - 2 * laplace; ++band)
                nFeatures++;
    nFeatures += 4; // The hough features
    
    std::vector<svt::BasicFV> testVectors(nuclei.size());

    for (size_t i = 0; i < testVectors.size(); ++i)
        testVectors[i].resize(nFeatures);
    
    if (pr != NULL && !pr->updateProgress(2)) return;

    // Check whether features need to be recomputed
    bool needsFeatureUpdate = forceFeatureComputation;
    if (needsFeatureUpdate)
    {
      for (ptrdiff_t j = 0; j < static_cast<ptrdiff_t>(nuclei.size()); ++j)
      {
        nuclei[j].features().resize(nFeatures);
        nuclei[j].setNeedsFeatureUpdate(true);
      }
    }
    else
    {
      for (ptrdiff_t j = 0; j < static_cast<ptrdiff_t>(nuclei.size()); ++j)
      {
        if (static_cast<int>(nuclei[j].features().size()) != nFeatures)
        {
          nuclei[j].features().resize(nFeatures);
          nuclei[j].setNeedsFeatureUpdate(true);
        }
        needsFeatureUpdate |= nuclei[j].needsFeatureUpdate();
      }
    }
    
    if (pr != NULL && !pr->updateProgress(3)) return;

    double progressPerChunk = 93.0;
    double progressLoadFeatures =
        ((needsFeatureUpdate) ? 0.8 : 0.1) * progressPerChunk;
    double progressStepPerFeatureLoad =
        progressLoadFeatures / static_cast<double>(nFeatures);
    double progressClassify = progressPerChunk - progressLoadFeatures;

    double progress = pr->progress();
    
    if (needsFeatureUpdate)
    {
      if (pr != NULL)
      {
        pr->updateProgressMessage("Preparing feature vectors");
        if (!pr->updateProgress(4)) return;
      }
      
      // Compute SD features
      int feaIdx = 0;
      for (double sigma = sigmaMin; sigma <= sigmaMax; sigma *= sigmaStep)
      {
        for (int laplace = 0; laplace <= bandMax / 2; ++laplace)
        {
          int maxBand = bandMax - 2 * laplace;
          for (int band = 0; band <= maxBand; ++band, ++feaIdx)
          {
            if (pr != NULL && pr->isAborted()) return;
            atb::Array<double,3> &fea = features.sdFeature(
                data, atb::SDMagFeatureIndex(sigma, laplace, band), maxBand,
                cacheFileName);
          
            atb::LinearInterpolator<double,3> ip(atb::ValueBT);
#ifdef _OPENMP
#pragma omp parallel for
#endif
            for (ptrdiff_t j = 0; j < static_cast<ptrdiff_t>(nuclei.size());
                 ++j)
            {
              if (!nuclei[j].needsFeatureUpdate() ||
                  (pr != NULL && pr->isAborted())) continue;
              blitz::TinyVector<double,3> pos(
                  nuclei[j].positionUm() / features.elementSizeUm());
              nuclei[j].features()[feaIdx] = ip.get(fea, pos);
            }
            
            progress += progressStepPerFeatureLoad;
            if (pr != NULL) pr->updateProgress(static_cast<int>(progress));
          }
          for (int band = 1; band <= bandMax - 2 * laplace; ++band)
              features.deleteFeature(
                  atb::SDMagFeatureIndex(sigma, laplace, band));
        }
        for (int laplace = 0; laplace <= bandMax / 2; ++laplace)
            features.deleteFeature(atb::SDMagFeatureIndex(sigma, laplace, 0));
        features.deleteFeature(atb::SDMagFeatureIndex(sigma, 0, 0));
      }
      
      // Compute hough features
      for (int i = iRoCS::Features::PositiveMagnitude;
           i <= iRoCS::Features::NegativeRadius; ++i, ++feaIdx)
      {
        atb::Array<double,3> &fea =
            features.houghFeature(data, i, cacheFileName);
        
        atb::LinearInterpolator<double,3> ip(atb::ValueBT);
#ifdef _OPENMP
#pragma omp parallel for
#endif
        for (ptrdiff_t j = 0; j < static_cast<ptrdiff_t>(nuclei.size()); ++j)
        {
          if (!nuclei[j].needsFeatureUpdate() ||
              (pr != NULL && pr->isAborted())) continue;
          blitz::TinyVector<double,3> pos(
              nuclei[j].positionUm() / features.elementSizeUm());
          nuclei[j].features()[feaIdx] = ip.get(fea, pos);
        }
        
        features.deleteFeature(i);
        progress += progressStepPerFeatureLoad;
        
        if (pr != NULL &&
            !pr->updateProgress(static_cast<int>(progress))) return;
      }
    }
    else
    {
      progress += progressLoadFeatures;
      if (pr != NULL && !pr->updateProgress(static_cast<int>(progress))) return;
    }

#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (ptrdiff_t j = 0; j < static_cast<ptrdiff_t>(nuclei.size()); ++j)
    {
      for (int i = 0; i < static_cast<int>(nFeatures); ++i)
          testVectors[j][i] = nuclei[j].features()[i];
      nuclei[j].setNeedsFeatureUpdate(false);
    }

    // Normalize features
    if (pr != NULL && !pr->updateProgressMessage("Normalizing features"))
        return;
    features.normalizeFeatures(testVectors);

    progress += 1.0;
    if (pr != NULL && !pr->updateProgress(static_cast<int>(progress))) return;

    if (pr != NULL) pr->setTaskProgressRange(
        static_cast<int>(
            5.0 + progressPerChunk + progressLoadFeatures),
        static_cast<int>(
            5.0 + progressPerChunk + progressLoadFeatures + progressClassify));
    
    features.classifyMultiClassSVM(testVectors, modelFileName);

    // Set nucleus labels according to classification
#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (ptrdiff_t j = 0; j < static_cast<ptrdiff_t>(nuclei.size()); ++j)
    {
      // Set predicted label and label if not manually set
      int label = static_cast<int>(testVectors[j].getLabel()) % 10;
      bool mitotic = static_cast<int>(testVectors[j].getLabel()) > 10;
      if (label == 2 || label == 0) nuclei[j].setPredictedLabel(label);
      else nuclei[j].setPredictedLabel(-1);
      nuclei[j].setPredictedMitotic(mitotic);
      if (!nuclei[j].manual())
      {
        if (label == 2 || label == 0) nuclei[j].setLabel(label);
        else nuclei[j].setLabel(-1);
        nuclei[j].setMitotic(mitotic);
      }
    }
  }

}
