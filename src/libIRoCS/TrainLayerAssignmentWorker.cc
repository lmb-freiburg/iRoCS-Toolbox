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

#include "TrainLayerAssignmentWorker.hh"

#include <libsvmtl/StDataHdf5.hh>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#endif 

#include "TrainfileParameters.hh"
#include "iRoCSFeatures.hh"

namespace iRoCS
{

  void trainLayerAssignment(
      TrainingParameters const &parameters, ProgressReporter *pr)
  {
    double sigmaMin = 0.5;
    double sigmaMax = 64.0;
    double sigmaStep = 2.0;
    int bandMax = 5;

    Features features(blitz::TinyVector<double,3>(1.0), pr);

    if (pr != NULL) pr->setProgressMin(0);
  
    int nFeatures = 0;
    for (double sigma = sigmaMin; sigma <= sigmaMax; sigma *= sigmaStep)
        for (int laplace = 0; laplace <= bandMax / 2; ++laplace)
            for (int band = 0; band <= bandMax - 2 * laplace; ++band)
                nFeatures++;
    nFeatures += 4; // The hough features
    
    std::string sdMagGroup = BlitzH5File::simplifyGroupDescriptor(
        parameters.featureGroup() + "/SDmag");
    for (double sigma = sigmaMin; sigma <= sigmaMax; sigma *= sigmaStep)
        for (int laplace = 0; laplace <= bandMax / 2; ++laplace)
            for (int band = 0; band <= bandMax - 2 * laplace; ++band)
                features.addFeatureToGroup(
                    sdMagGroup, features.sdFeatureName(
                        atb::SDMagFeatureIndex(sigma, laplace, band)));
    features.setGroupNormalization(
        sdMagGroup, parameters.sdFeatureNormalization());
  
    std::string houghGroup = BlitzH5File::simplifyGroupDescriptor(
        parameters.featureGroup() + "/hough");
    for (int i = iRoCS::Features::PositiveMagnitude;
         i <= iRoCS::Features::NegativeRadius; ++i)
        features.addFeatureToGroup(houghGroup, features.houghFeatureName(i)); 
    features.setGroupNormalization(
        houghGroup, parameters.houghFeatureNormalization());

    std::string iRoCSGroup = BlitzH5File::simplifyGroupDescriptor(
        parameters.featureGroup() + "/intrinsicCoordinates");
    features.addFeatureToGroup(iRoCSGroup, "/phi");
    features.addFeatureToGroup(iRoCSGroup, "/qcDistance_um");
    features.addFeatureToGroup(iRoCSGroup, "/radialDistance_um");
    features.setGroupNormalization(iRoCSGroup, iRoCS::Features::None);
  
    std::vector<svt::BasicFV> trainingSet;
    int trainIndex = 0;

    std::vector<TrainfileParameters*> trainFiles(parameters.trainFiles());
    if (pr != NULL)
        pr->setProgressMax(100 * static_cast<int>(trainFiles.size()) + 10);
    for (size_t fileIdx = 0; fileIdx < trainFiles.size(); ++fileIdx)
    {
      if (pr != NULL)
      {
        if (!pr->updateProgressMessage(
                "Processing '" + trainFiles[fileIdx]->trainFileName() + "'"))
            return;
        pr->updateProgress(static_cast<int>(fileIdx) * 100);
      }

      // Read dataset
      atb::Array<double,3> data;
      try
      {
        std::cout << "  Loading '" << trainFiles[fileIdx]->trainFileName()
                  << ":" << trainFiles[fileIdx]->dataChannelName() << "'... "
                  << std::flush;
        data.load(
            trainFiles[fileIdx]->trainFileName(),
            trainFiles[fileIdx]->dataChannelName());
        std::cout << "OK" << std::endl;
      }
      catch (BlitzH5Error &e)
      {
        std::cout << "failed" << std::endl;
        if (pr != NULL)
            pr->abortWithError(
                "Could not read '" + trainFiles[fileIdx]->trainFileName() +
                ":" + trainFiles[fileIdx]->dataChannelName() + "': " +
                e.what());
        else
            std::cerr << "Could not read '"
                      << trainFiles[fileIdx]->trainFileName() << ":"
                      << trainFiles[fileIdx]->dataChannelName() << "': "
                      << e.what() << std::endl;
        return;
      }

      if (pr != NULL &&
          !pr->updateProgress(static_cast<int>(fileIdx) * 100 + 3)) return;

      // Read markers
      std::cout << "  Loading '" << trainFiles[fileIdx]->trainFileName() << ":"
                << trainFiles[fileIdx]->annotationChannelName() << "'... "
                << std::flush;
      std::vector<atb::Nucleus> nuclei;
      try
      {
        BlitzH5File inFile(trainFiles[fileIdx]->trainFileName());
        atb::Nucleus::loadList(
            nuclei, inFile, trainFiles[fileIdx]->annotationChannelName());
        std::cout << "OK" << std::endl;
      }
      catch (std::exception &e)
      {
        std::cout << "failed" << std::endl;
        if (pr != NULL)
            pr->abortWithError(
                "Could not read '" + trainFiles[fileIdx]->trainFileName() +
                ":" + trainFiles[fileIdx]->annotationChannelName() + "': " +
                e.what());
        else
            std::cerr << "Could not read '"
                      << trainFiles[fileIdx]->trainFileName() << ":"
                      << trainFiles[fileIdx]->annotationChannelName() << "': "
                      << e.what() << std::endl;
        return;
      }
      
      if (pr != NULL &&
          !pr->updateProgress(static_cast<int>(fileIdx) * 100 + 4)) return;

      // Generate Random Samples
      if (parameters.generateRandomSamples())
      {
        std::cout << "Generating random samples... " << std::flush;
        size_t nMarkers = nuclei.size();
        std::vector< blitz::TinyVector<double,3> > markers(nMarkers);
        for (size_t i = 0; i < nMarkers; ++i)
            markers[i] = nuclei[i].positionUm();
        features.generateRandomSamples(
            markers, blitz::TinyVector<double,3>(
                blitz::TinyVector<double,3>(data.shape()) *
                data.elementSizeUm()), parameters.nInRootSamples(),
            parameters.nOutRootSamples());
        for (size_t i = nMarkers; i < markers.size(); ++i)
        {
          atb::Nucleus nc;
          nc.setPositionUm(markers[i]);
          nc.setRadiusUm(2.0);
          nc.setLabel(0);
          nuclei.push_back(nc);
        }
        std::cout << "OK" << std::endl;
      }

      if (pr != NULL &&
          !pr->updateProgress(static_cast<int>(fileIdx) * 100 + 8)) return;

      // Generate Features
      for (size_t i = 0; i < nuclei.size(); ++i)
          nuclei[i].features().resize(nFeatures + 3);

      int feaIdx = 0;
      for (double sigma = sigmaMin; sigma <= sigmaMax; sigma *= sigmaStep)
      {
        for (int laplace = 0; laplace <= bandMax / 2; ++laplace)
        {
          int maxBand = bandMax - 2 * laplace;
          for (int band = 0; band <= maxBand; ++band, ++feaIdx)
          {  
            if (pr != NULL && !pr->updateProgress(
                    static_cast<int>(fileIdx) * 100 + 8 +
                    static_cast<int>(82.0 * static_cast<double>(feaIdx) /
                                     static_cast<double>(nFeatures - 4))))
                continue;
            blitz::Array<double,3>& fea = features.sdFeature(
                data, atb::SDMagFeatureIndex(sigma, laplace, band), maxBand,
                trainFiles[fileIdx]->cacheFileName());

            atb::LinearInterpolator<double,3> ip(atb::ValueBT);

#ifdef _OPENMP
#pragma omp parallel for
#endif
            for (ptrdiff_t k = 0; k < static_cast<ptrdiff_t>(nuclei.size());
                 ++k)
            {
              if (!nuclei[k].needsFeatureUpdate() ||
                  (pr != NULL && pr->isAborted())) continue;
              blitz::TinyVector<double,3> pos(
                  nuclei[k].positionUm() / features.elementSizeUm());
              nuclei[k].features()[feaIdx] = ip.get(fea, pos);
            }
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
        if (pr != NULL && !pr->updateProgress(
                static_cast<int>(fileIdx) * 100 + 90 + 2 * i)) continue;
        blitz::Array<double,3>& fea =
            features.houghFeature(
                data, i, trainFiles[fileIdx]->cacheFileName());
        if (pr->isAborted()) continue;

        atb::LinearInterpolator<double,3> ip(atb::ValueBT);

#ifdef _OPENMP
#pragma omp parallel for
#endif
        for (ptrdiff_t k = 0; k < static_cast<ptrdiff_t>(nuclei.size()); ++k)
        {
          if (!nuclei[k].needsFeatureUpdate() ||
              (pr != NULL && pr->isAborted())) continue;
          blitz::TinyVector<double,3> pos(
              nuclei[k].positionUm() / features.elementSizeUm());
          nuclei[k].features()[feaIdx] = ip.get(fea, pos);
        }
        features.deleteFeature(i);
        if (pr != NULL && pr->isAborted()) return;
      }

      // Append intrinsic coordinates
      atb::IRoCS rct;
      rct.load(trainFiles[fileIdx]->trainFileName(),
               trainFiles[fileIdx]->iRoCSChannelName());
#ifdef _OPENMP
#pragma omp parallel for
#endif
      for (ptrdiff_t k = 0; k < static_cast<ptrdiff_t>(nuclei.size()); ++k)
      {
        if (pr != NULL && pr->isAborted()) continue;
        blitz::TinyVector<double,3> pos(
            rct.getCoordinates(nuclei[k].positionUm()));
        nuclei[k].features()[feaIdx] = pos(2);
        nuclei[k].features()[feaIdx + 1] = pos(0);
        nuclei[k].features()[feaIdx + 2] = pos(1);
      }    

      // Append samples to training set
      for (size_t k = 0; k < nuclei.size(); ++k)
      {
        if (nuclei[k].label() != -1)
        {
          int label = nuclei[k].label() + ((nuclei[k].mitotic()) ? 10 : 0);
          trainingSet.push_back(
              svt::BasicFV(nuclei[k].features(), label, trainIndex));
          trainIndex++;
        }
      }
    }

    // Normalize features
    if (pr != NULL && !pr->updateProgressMessage("Normalizing features"))
        return;
    features.normalizeFeatures(trainingSet);

    if (pr != NULL && !pr->updateProgress(
            100 * static_cast<int>(trainFiles.size()) + 8)) return;

    // Create SVM model
    try
    {
      try
      {
        BlitzH5File modelFile(parameters.modelFileName(), BlitzH5File::Replace);
      }
      catch (BlitzH5Error &e)
      {
        std::cout << "Could not create SVM model file '"
                  << parameters.modelFileName() << "': " << e.what()
                  << std::endl;
        if (pr != NULL) pr->abortWithError(
            std::string("Error while creating SVM model: ") + e.what());
      }
      svt::StDataHdf5 modelMap(
          parameters.modelFileName().c_str(), H5F_ACC_RDWR);
    }
    catch (std::exception& e)
    {
      std::cout << "Could not create SVM model file '"
                << parameters.modelFileName() << "': " << e.what() << std::endl;
      if (pr != NULL) pr->abortWithError(
          std::string("Error while creating SVM model: ") + e.what());
    }
    if (pr != NULL && pr->isAborted()) return;

    features.saveNormalizationParameters(parameters.modelFileName());

    // Train svm
    if (pr != NULL)
    {
      if (!pr->updateProgress(
              100 * static_cast<int>(trainFiles.size()) + 8)) return;
      pr->updateProgressMessage(
          "Training SVM (This may take hours or even days...)");
      pr->setProgressMin(0);
      pr->setProgressMax(0);
    }
    features.trainMultiClassSVM(
        trainingSet, parameters.modelFileName(),
        static_cast<float>(parameters.cost()),
        static_cast<float>(parameters.gamma()));
  }

}
