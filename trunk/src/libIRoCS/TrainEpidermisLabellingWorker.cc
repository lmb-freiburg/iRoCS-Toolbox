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

#include "TrainEpidermisLabellingWorker.hh"

#include <libsvmtl/StDataHdf5.hh>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#endif 

#include "TrainfileParameters.hh"

#include <liblabelling_qt4/LabellingMainWidget.hh>
#include <liblabelling_qt4/NucleusMarker.hh>

TrainEpidermisLabellingWorker::TrainEpidermisLabellingWorker(
    TrainingParameters const &parameters,
    iRoCS::ProgressReporter *progress) 
        : QThread(), _parameters(parameters), p_progress(progress),
          _sigmaMin(0.5), _sigmaMax(64.0), _sigmaStep(2.0), _bandMax(5)
{
  p_features = new iRoCS::Features(
      blitz::TinyVector<double,3>(1.0), p_progress);
  p_annotation = new AnnotationChannelSpecs(Marker::Sphere);
}

TrainEpidermisLabellingWorker::~TrainEpidermisLabellingWorker()
{
  delete p_features;
  delete p_annotation;
}

void TrainEpidermisLabellingWorker::run()
{
  if (p_progress != NULL) p_progress->setProgressMin(0);
  
  int nFeatures = 0;
  for (double sigma = _sigmaMin; sigma <= _sigmaMax; sigma *= _sigmaStep)
      for (int laplace = 0; laplace <= _bandMax / 2; ++laplace)
          for (int band = 0; band <= _bandMax - 2 * laplace; ++band)
              nFeatures++;
  nFeatures += 4; // The hough features
    
  std::string sdMagGroup = BlitzH5File::simplifyGroupDescriptor(
      _parameters.featureGroup() + "/SDmag");
  for (double sigma = _sigmaMin; sigma <= _sigmaMax; sigma *= _sigmaStep)
      for (int laplace = 0; laplace <= _bandMax / 2; ++laplace)
          for (int band = 0; band <= _bandMax - 2 * laplace; ++band)
              p_features->addFeatureToGroup(
                  sdMagGroup, p_features->sdFeatureName(
                      atb::SDMagFeatureIndex(sigma, laplace, band)));
  p_features->setGroupNormalization(
      sdMagGroup, _parameters.sdFeatureNormalization());
  
  std::string houghGroup = BlitzH5File::simplifyGroupDescriptor(
      _parameters.featureGroup() + "/hough");
  for (int i = iRoCS::Features::PositiveMagnitude;
       i <= iRoCS::Features::NegativeRadius; ++i)
  {
    p_features->addFeatureToGroup(
        houghGroup, p_features->houghFeatureName(i)); 
  }
  p_features->setGroupNormalization(
      houghGroup, _parameters.houghFeatureNormalization());

  std::vector<svt::BasicFV> trainingSet;
  int trainIndex = 0;
  int nEpidermis = 0;
  int nNoNucleus = 0;
  int nMitoses = 0;
  int nOtherNucleus = 0;

  std::vector<TrainfileParameters*> trainFiles(
      _parameters.trainFiles());
  if (p_progress != NULL)
      p_progress->setProgressMax(
          100 * static_cast<int>(trainFiles.size()) + 10);
  for (size_t fileIdx = 0; fileIdx < trainFiles.size(); ++fileIdx)
  {
    std::cerr << "Processing " << trainFiles[fileIdx]->trainFileName()
              << std::endl;
    if (p_progress != NULL && !p_progress->updateProgressMessage(
            "Processing '" + trainFiles[fileIdx]->trainFileName() + "'"))
        return;
    std::cout << "Processing '" << trainFiles[fileIdx]->trainFileName() << "'"
              << std::endl;

    if (p_progress != NULL &&
        !p_progress->updateProgress(static_cast<int>(fileIdx) * 100)) return;

    // Read dataset
    atb::Array<double,3> data;
    try
    {
      std::cout << "  Loading '" << trainFiles[fileIdx]->trainFileName() << ":"
                << trainFiles[fileIdx]->dataChannelName() << "'... "
                << std::flush;
      data.load(
          trainFiles[fileIdx]->trainFileName(),
          trainFiles[fileIdx]->dataChannelName());
      std::cout << "OK" << std::endl;
    }
    catch (BlitzH5Error &e)
    {
      std::cout << "failed" << std::endl;
      if (p_progress != NULL)
          p_progress->abortWithError(
              QObject::tr("Could not read '%1:%2': %3").arg(
                  trainFiles[fileIdx]->trainFileName().c_str()).arg(
                      trainFiles[fileIdx]->dataChannelName().c_str()).arg(
                          e.what()).toStdString());
      return;
    }

    if (p_progress != NULL &&
        !p_progress->updateProgress(static_cast<int>(fileIdx) * 100 + 3))
        return;

    // Read markers
    std::cout << "  Loading '" << trainFiles[fileIdx]->trainFileName() << ":"
              << trainFiles[fileIdx]->annotationChannelName() << "'... "
              << std::flush;
    p_annotation->setName(trainFiles[fileIdx]->annotationChannelName());
    HDF5DataIO::RetVal res =
        p_annotation->load(trainFiles[fileIdx]->trainFileName());
    if (res != HDF5DataIO::Ok)
    {
      std::cout << "failed" << std::endl;
      if (p_progress != NULL)
          p_progress->abortWithError(
              tr("Could not read '%1:%2'").arg(
                  trainFiles[fileIdx]->trainFileName().c_str()).arg(
                      trainFiles[fileIdx]->annotationChannelName().c_str()).toStdString());
      return;
    }
    std::cout << "OK" << std::endl;

    if (p_progress != NULL &&
        !p_progress->updateProgress(static_cast<int>(fileIdx) * 100 + 4))
        return;

    // Generate Random Samples
    if (_parameters.generateRandomSamples())
    {
      std::cout << "Generating random samples... " << std::flush;
      size_t nMarkers = p_annotation->markers().size();
      std::vector< blitz::TinyVector<double,3> > markers(nMarkers);
      for (size_t i = 0; i < nMarkers; ++i)
          markers[i] = p_annotation->markers()[i]->positionUm();
      p_features->generateRandomSamples(
          markers, blitz::TinyVector<double,3>(
              blitz::TinyVector<double,3>(data.shape()) * data.elementSizeUm()),
          _parameters.nInRootSamples(), _parameters.nOutRootSamples());
      for (size_t i = nMarkers; i < markers.size(); ++i)
      {
        Marker *marker = p_annotation->addMarker(markers[i]);
        if (marker != NULL)
        {
          marker->setLabel(0);
          if (marker->inherits(Marker::Sphere))
              static_cast<SphereMarker*>(marker)->setRadiusUm(2.0);
        }
      }
      std::cout << "OK" << std::endl;
    }
    
    if (p_progress != NULL &&
        !p_progress->updateProgress(static_cast<int>(fileIdx) * 100 + 8))
        return;

    // Generate Features
    p_annotation->resizeFeatures(nFeatures);

    int feaIdx = 0;
    for (double sigma = _sigmaMin; sigma <= _sigmaMax; sigma *= _sigmaStep)
    {
      for (int laplace = 0; laplace <= _bandMax / 2; ++laplace)
      {
        int maxBand = _bandMax - 2 * laplace;
        for (int band = 0; band <= maxBand; ++band, ++feaIdx)
        {  
          if (p_progress != NULL && !p_progress->updateProgress(
                  static_cast<int>(fileIdx) * 100 + 8 +
                  static_cast<int>(82.0 * static_cast<double>(feaIdx) /
                                   static_cast<double>(nFeatures - 4))))
              continue;
          blitz::Array<double,3>& fea = p_features->sdFeature(
              data, atb::SDMagFeatureIndex(sigma, laplace, band), maxBand,
              trainFiles[fileIdx]->cacheFileName());

          atb::LinearInterpolator<double,3> ip(atb::ValueBT);

#ifdef _OPENMP
#pragma omp parallel for
#endif
          for (ptrdiff_t k = 0;
               k < static_cast<ptrdiff_t>(p_annotation->markers().size()); ++k)
          {
            Marker *marker = (*p_annotation)[k];
            if (!marker->needsFeatureUpdate() ||
                (p_progress != NULL && p_progress->isAborted())) continue;
            blitz::TinyVector<double,3> pos(
                marker->positionUm() / p_features->elementSizeUm());
            marker->feature(feaIdx) = ip.get(fea, pos);
          }
        }
        for (int band = 1; band <= _bandMax - 2 * laplace; ++band)
            p_features->deleteFeature(
                atb::SDMagFeatureIndex(sigma, laplace, band));
      }
      for (int laplace = 0; laplace <= _bandMax / 2; ++laplace)
          p_features->deleteFeature(atb::SDMagFeatureIndex(sigma, laplace, 0));
      p_features->deleteFeature(atb::SDMagFeatureIndex(sigma, 0, 0));
    }

    // Compute hough features
    for (int i = iRoCS::Features::PositiveMagnitude;
         i <= iRoCS::Features::NegativeRadius; ++i, ++feaIdx)
    {
      if (p_progress != NULL && !p_progress->updateProgress(
              static_cast<int>(fileIdx) * 100 + 90 + 2 * i)) continue;
      blitz::Array<double,3>& fea =
          p_features->houghFeature(
              data, i, trainFiles[fileIdx]->cacheFileName());
      if (p_progress->isAborted()) continue;

      atb::LinearInterpolator<double,3> ip(atb::ValueBT);

#ifdef _OPENMP
#pragma omp parallel for
#endif
      for (ptrdiff_t k = 0;
           k < static_cast<ptrdiff_t>(p_annotation->markers().size()); ++k)
      {
        Marker *marker = (*p_annotation)[k];
        if (!marker->needsFeatureUpdate() ||
            (p_progress != NULL && p_progress->isAborted())) continue;
        blitz::TinyVector<double,3> pos(
            marker->positionUm() / p_features->elementSizeUm());
        marker->feature(feaIdx) = ip.get(fea, pos);
      }
      p_features->deleteFeature(i);
      if (p_progress != NULL && p_progress->isAborted()) return;
    }

    // Append samples to training set
    for (std::vector<Marker*>::const_iterator it = p_annotation->begin();
         it != p_annotation->end(); ++it)
    {
      if ((*it)->label() != -1)
      {
        int label = ((*it)->label() != 0 && (*it)->label() != 2) ?
            -1 : (*it)->label();
        if ((*it)->markerType() == Marker::Nucleus &&
            static_cast<NucleusMarker*>(*it)->mitotic()) label = 14;
        trainingSet.push_back(
            svt::BasicFV((*it)->features(), label, trainIndex));
        switch (static_cast<int>(trainingSet.back().getLabel()))
        {
        case -1:
          nOtherNucleus++;
          break;
        case 0:
          nNoNucleus++;
          break;
        case 2:
          nEpidermis++;
          break;
        case 14:
          nMitoses++;
          break;
        default:
          std::cerr << __FILE__ << ":" << __LINE__
                    << " - Found unknown class "
                    << trainingSet.back().getLabel()
                    << " - Please report this bug." << std::endl;
          return;
        }
        trainIndex++;
      }
    }
  }
  if (nEpidermis == 0)
  {
    if (p_progress != NULL)
        p_progress->abortWithError(
            tr("The given annotation does not contain epidermis interphase "
               "samples. Discriminative training impossible!").toStdString());
    return;    
  }
  if ((nOtherNucleus + nNoNucleus + nMitoses) == 0)
  {
    if (p_progress != NULL)
        p_progress->abortWithError(
            tr("The given annotation only contains Epidermis samples. "
               "Please also provide some negative examples. Discriminative "
               "training impossible!").toStdString());
    return;
  }

  // Normalize features
  if (p_progress != NULL && !p_progress->updateProgressMessage(
          tr("Normalizing features").toStdString())) return;
  p_features->normalizeFeatures(trainingSet);

// #define TRAINEPIDERMISLABELLINGDEBUG
#ifdef TRAINEPIDERMISLABELLINGDEBUG
  {
    // DEBUG save of training set
    std::ofstream trainFile(
        (_parameters.modelFileName().substr(
            0, _parameters.modelFileName().rfind(".")) + ".train").c_str(),
        std::ios::trunc);
    for (size_t i = 0; i < trainingSet.size(); ++i)
    {
      trainFile << trainingSet[i].getLabel();
      for (size_t j = 0; j < trainingSet[i].size(); ++j)
          trainFile << " " << trainingSet[i][j];
      trainFile << "\n";
    }
  }
#endif

  if (p_progress != NULL && !p_progress->updateProgress(
          100 * static_cast<int>(trainFiles.size()) + 8)) return;

  // Create SVM model
  try
  {
    try
    {
      BlitzH5File modelFile(_parameters.modelFileName(), BlitzH5File::Replace);
    }
    catch (BlitzH5Error &e)
    {
      std::cout << "Could not create SVM model file '"
                << _parameters.modelFileName() << "': " << e.what()
                << std::endl;
      if (p_progress != NULL) p_progress->abortWithError(
          tr("Error while creating SVM model: %1").arg(e.what()).toStdString());
    }
    svt::StDataHdf5 modelMap(_parameters.modelFileName().c_str(), H5F_ACC_RDWR);
  }
  catch (std::exception& e)
  {
    std::cout << "Could not create SVM model file '"
              << _parameters.modelFileName() << "': " << e.what() << std::endl;
    if (p_progress != NULL) p_progress->abortWithError(
        tr("Error while creating SVM model: %1").arg(e.what()).toStdString());
  }
  if (p_progress != NULL && p_progress->isAborted()) return;

  p_features->saveNormalizationParameters(_parameters.modelFileName());

  if (!p_progress->updateProgress(75)) return;

  // Train svm
  if (p_progress != NULL)
  {
    if (!p_progress->updateProgress(
            100 * static_cast<int>(trainFiles.size()) + 8)) return;
    p_progress->updateProgressMessage(
        tr("Training SVM (This may take hours...)").toStdString());
    p_progress->setProgressMin(0);
    p_progress->setProgressMax(0);
  }

  p_features->trainMultiClassSVM(
      trainingSet, _parameters.modelFileName(),
      static_cast<float>(_parameters.cost()),
      static_cast<float>(_parameters.gamma()));
}
