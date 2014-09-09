#include "iRoCSFeatures.hh"

#include <liblabelling_qt4/SphereMarker.hh>

#include <libArrayToolbox/Random.hh>

#include <libsvmtl/StDataHdf5.hh>
#include <libsvmtl/MultiClassSVMOneVsOne.hh>
#include <libsvmtl/TwoClassSVMc.hh>
#include <libsvmtl/Kernel_RBF.hh>
#include <libsvmtl/Model.hh>

namespace iRoCS
{

  std::string Features::normalizationTypeToString(NormalizationType type)
  {
    switch (type)
    {
    case None :
      return "No normalization";
    case FeatureZeroMeanStddev :
      return "Feature-wise Zero Mean and Unit Standard deviation";
    case UnitNorm :
      return "Unit feature vector norm";
    case Combined :
      return "Unit vector norm + individual feature normalization";
    default :
      return "Invalid";
    }
    return "Invalid";
  }
  
  Features::NormalizationType Features::stringToNormalizationType(
      std::string const &normTypeString)
  {
    if (normTypeString == normalizationTypeToString(None))
        return None;
    else if (normTypeString == normalizationTypeToString(FeatureZeroMeanStddev))
        return FeatureZeroMeanStddev;
    else if (normTypeString == normalizationTypeToString(UnitNorm))
        return UnitNorm;
    else if (normTypeString == normalizationTypeToString(Combined))
        return Combined;
    return Invalid;
  }

  const int Features::PositiveMagnitude = 0x001;
  const int Features::NegativeMagnitude = 0x002;
  const int Features::PositiveRadius = 0x003;
  const int Features::NegativeRadius = 0x004;

  Features::Features(
      blitz::TinyVector<double,3> const &featureElementSizeUm,
      iRoCS::ProgressReporter *progress)
          : p_progress(progress)
  {
    std::cout << "Initializing iRoCS::Features... " << std::flush;
    _dataScaled.setElementSizeUm(featureElementSizeUm);
    _intrinsicCoordinates.setElementSizeUm(featureElementSizeUm);
    _houghDsNames[PositiveMagnitude] = "houghmap1";
    _houghDsNames[NegativeMagnitude] = "houghmap2";
    _houghDsNames[PositiveRadius] = "houghmapR1";
    _houghDsNames[NegativeRadius] = "houghmapR2";    
    std::cout << "OK" << std::endl;
  }

  Features::~Features()
  {
    std::cout << "Cleaning up iRoCS::Features... " << std::flush;
    for (std::map<atb::SDMagFeatureIndex,atb::Array<double,3>*>::iterator it =
             _sdFeatures.begin(); it != _sdFeatures.end(); ++it)
        delete it->second;
    for (std::map<int,atb::Array<double,3>*>::iterator it =
             _houghFeatures.begin(); it != _houghFeatures.end(); ++it)
        delete it->second;
    for (std::vector<std::vector<std::string>*>::iterator it =
             _featureNames.begin(); it != _featureNames.end(); ++it)
        delete *it;
    for (std::vector<std::vector<double>*>::iterator it = _means.begin();
         it != _means.end(); ++it) delete *it;
    for (std::vector<std::vector<double>*>::iterator it = _stddevs.begin();
         it != _stddevs.end(); ++it) delete *it;
    std::cout << "OK" << std::endl;
  }

  blitz::TinyVector<double,3> const &Features::elementSizeUm() const
  {
    return _dataScaled.elementSizeUm();
  }

  void Features::addFeatureToGroup(
      std::string const &groupName, std::string const &featureName)
  {
    size_t i = 0;
    for (; i < _featureGroups.size() && _featureGroups[i] != groupName; ++i);
    if (i == _featureGroups.size())
    {
      _featureGroups.push_back(groupName);
      _normalize.push_back(None);
      _featureNames.push_back(new std::vector<std::string>());
    }
    _featureNames[i]->push_back(featureName);
  }

  void Features::setGroupNormalization(
      std::string const &groupName, NormalizationType normType)
  {
    size_t i = 0;
    for (; i < _featureGroups.size() && _featureGroups[i] != groupName; ++i);
    if (i < _featureGroups.size()) _normalize[i] = normType;
  }

  std::string Features::sdFeatureName(atb::SDMagFeatureIndex const &index) const
  {
    std::stringstream dsStream;
    dsStream << "sigma" << std::setw(4) << std::setfill('0') << index.s
             << "_laplace" << std::setw(4) << std::setfill('0') << index.l
             << "_l" << std::setw(4) << std::setfill('0') << index.b;
    return dsStream.str();
  }

  std::string Features::houghFeatureName(const int state) const
  {
    return _houghDsNames.find(state)->second;
  }

  void Features::deleteFeature(atb::SDMagFeatureIndex const &index)
  {
    if (_sdFeatures.find(index) != _sdFeatures.end())
    {
      std::cout << "Removing '" << sdFeatureName(index) << "' from cache... "
                << std::flush;
      delete _sdFeatures[index];
      _sdFeatures.erase(index);
      std::cout << "OK" << std::endl;
    }
  }

  void Features::deleteFeature(const int state)
  {
    if (_houghFeatures.find(state) != _houghFeatures.end())
    {
      std::cout << "Removing '" << houghFeatureName(state)
                << "' from cache... " << std::flush;
      delete _houghFeatures[state];
      _houghFeatures.erase(state);
      std::cout << "OK" << std::endl;
    }
  }

  void Features::generateRandomSamples(
      std::vector< blitz::TinyVector<double,3> > &markers,
      blitz::TinyVector<double,3> const &upperBoundUm,
      int nInRootSamples, int nOutRootSamples)
  {
    if (p_progress != NULL && !p_progress->updateProgressMessage(
            "Computing distribution parameters"))
        return;
    blitz::TinyVector<double,3> mean = 0.0;
    blitz::TinyMatrix<double,3,3> cov;
    cov = 0.0;
    for (size_t i = 0; i < markers.size() &&
             (p_progress == NULL || !p_progress->isAborted()); ++i)
        mean += markers[i];
    mean /= static_cast<double>(markers.size());

    for (size_t i = 0; i < markers.size() &&
             (p_progress == NULL || !p_progress->isAborted()); ++i)
    {
      blitz::TinyVector<double,3> p = markers[i] - mean;
      cov = cov + atb::outerProduct(p, p);
    }
    cov = 1.0 / static_cast<double>(markers.size()) * cov;
  
    int sample = 0;
    while (sample < nInRootSamples) 
    {
      blitz::TinyVector<double,3> pos = atb::drawRandomVector(mean, cov);
      if (blitz::any(pos < 0 || pos >= upperBoundUm)) continue;

      size_t i = 0;
      for (; i < markers.size() &&
               (p_progress == NULL || !p_progress->isAborted()); ++i)
          if (blitz::dot(markers[i] - pos, markers[i] - pos) < 4.0) break;

      if (i == markers.size())
      {
        markers.push_back(pos);
        sample++;
        std::stringstream msg;
        msg << "Generating in-root samples (" << sample << " / "
            << nInRootSamples << ")";
        if (p_progress != NULL && !p_progress->updateProgressMessage(
                msg.str())) return;
      }
    }
  
    sample = 0;
    while (sample < nOutRootSamples) 
    {
      blitz::TinyVector<double,3> pos = atb::drawRandomVector(upperBoundUm);

      size_t i = 0;
      for (; i < markers.size() &&
               (p_progress == NULL || !p_progress->isAborted()); ++i)
          if (blitz::dot(markers[i] - pos, markers[i] - pos) < 4.0) break;

      if (i == markers.size())
      {
        markers.push_back(pos);
        sample++;
        std::stringstream msg;
        msg << "Generating out-of-root samples (" << sample << " / "
            << nOutRootSamples << ")";
        if (p_progress != NULL && !p_progress->updateProgressMessage(
                msg.str())) return;
      }
    }
  }

  void Features::normalizeFeatures(std::vector<svt::BasicFV> &samples)
  {
    size_t feaStart = 0;
    for (size_t i = 0; i < _featureGroups.size(); ++i) 
    {
      if ((int(_normalize[i]) & UnitNorm) == UnitNorm) 
      {
        std::string msg("Normalizing '" + _featureGroups[i] + "' to unit norm");
        if (p_progress != NULL)
        {
          if (!p_progress->updateProgressMessage(msg)) return;
        }
        else std::cout << msg << std::endl;
        
#ifdef _OPENMP
#pragma omp parallel for
#endif
        for (ptrdiff_t k = 0; k < static_cast<ptrdiff_t>(samples.size()); ++k)
        {
          if (p_progress != NULL && p_progress->isAborted()) continue;
          double norm = 0.0;
          for (size_t j = 0; j < _featureNames[i]->size(); ++j)
              norm += samples[k][static_cast<int>(j + feaStart)] *
                  samples[k][static_cast<int>(j + feaStart)];
          norm = (norm > 0.0) ? 1.0 / std::sqrt(norm) : 0.0;
          for (size_t j = 0; j < _featureNames[i]->size(); ++j)
              samples[k][static_cast<int>(j + feaStart)] *= norm;
        }
        if (p_progress != NULL && p_progress->isAborted()) return;
      }
      feaStart += _featureNames[i]->size();
    }
    if (p_progress != NULL && p_progress->isAborted()) return;

    if (_means.size() == 0) 
    {
      if (p_progress != NULL && !p_progress->updateProgressMessage(
              "Computing feature means and standard deviations")) return;
      _means.resize(_featureGroups.size(), NULL);
      _stddevs.resize(_featureGroups.size(), NULL);
      size_t feaIdx = 0;
      for (size_t i = 0; i < _featureGroups.size(); ++i)
      {
        if (p_progress != NULL && p_progress->isAborted()) return;
        _means[i] = new std::vector<double>(_featureNames[i]->size(), 0.0);
        _stddevs[i] = new std::vector<double>(_featureNames[i]->size(), 0.0);

        for (ptrdiff_t j = 0;
             j < static_cast<ptrdiff_t>(_featureNames[i]->size()); ++j) 
        {
          if (p_progress != NULL && p_progress->isAborted()) continue;
          for (std::vector<svt::BasicFV>::const_iterator it = samples.begin();
               it != samples.end(); ++it)
          {
            if (p_progress != NULL && p_progress->isAborted()) continue;
            (*_means[i])[j] += (*it)[static_cast<int>(feaIdx)];
          }
          (*_means[i])[j] /= static_cast<double>(samples.size());
          for (std::vector<svt::BasicFV>::const_iterator it = samples.begin();
               it != samples.end(); ++it)
          {
            if (p_progress != NULL && p_progress->isAborted()) continue;
            (*_stddevs[i])[j] +=
                std::pow((*it)[static_cast<int>(feaIdx)] -
                         (*_means[i])[j], 2.0);
          }
          (*_stddevs[i])[j] =
              std::sqrt((*_stddevs[i])[j] /
                        static_cast<double>(samples.size()));
          ++feaIdx;
        }
        if (p_progress != NULL && p_progress->isAborted())
        {
          for (size_t i = 0; i < _featureGroups.size(); ++i)
          {
            delete _means[i];
            delete _stddevs[i];
          }
          return;
        }
      }
    }
  
    feaStart = 0;
    for (size_t i = 0; i < _featureGroups.size(); ++i) 
    {
      if ((int(_normalize[i]) & FeatureZeroMeanStddev) ==
          FeatureZeroMeanStddev) 
      {
        std::string msg(
            "Normalizing individual features of '" + _featureGroups[i] +
            "' to zero mean and unit standard deviation");
        if (p_progress != NULL)
        {
          if (!p_progress->updateProgressMessage(msg)) return;
        }
        else std::cout << msg << std::endl;

#ifdef _OPENMP
#pragma omp parallel for
#endif
        for (ptrdiff_t k = 0; k < static_cast<ptrdiff_t>(samples.size()); ++k)
        {
          if (p_progress != NULL && p_progress->isAborted()) continue;
          size_t feaIdx = feaStart;
          for (size_t j = 0; j < _featureNames[i]->size(); ++j, ++feaIdx) 
          {
            samples[k][static_cast<int>(feaIdx)] =
                (samples[k][static_cast<int>(feaIdx)] -
                 (*_means[i])[j]) / (*_stddevs[i])[j];
          }
        }
        if (p_progress != NULL && p_progress->isAborted()) return;
      }
      feaStart += _featureNames[i]->size();
    }
  }

  void Features::saveNormalizationParameters(std::string const &modelFileName)
  {
    try
    {
      svt::StDataHdf5 modelMap(modelFileName.c_str(), H5F_ACC_RDWR);
      modelMap.setExceptionFlag(true);

      modelMap.setArray("featureGroups",
                        _featureGroups.begin(), _featureGroups.size());
      std::vector<int> normalize(_normalize.size());
      for (size_t i = 0; i < _normalize.size(); ++i)
          normalize[i] = int(_normalize[i]);
      modelMap.setArray("featureNormalization",
                        normalize.begin(), normalize.size());
      for (size_t i = 0; i < _featureGroups.size(); ++i) 
      {
        std::string groupName = h5GroupName(_featureGroups[i]);
        modelMap.setArray("featureNames_" + groupName,
                          _featureNames[i]->begin(), _featureNames[i]->size());
        modelMap.setArray("featureMeans_" + groupName,
                          _means[i]->begin(), _means[i]->size());
        modelMap.setArray("featureStddevs_" + groupName,
                          _stddevs[i]->begin(), _stddevs[i]->size());
      }
    }
    catch (std::exception &e)
    {
      if (p_progress != NULL)
          p_progress->abortWithError(
              std::string("Could not save normalization parameters to SVM "
                          "model: ") + e.what());
    }
  }

  void Features::loadNormalizationParameters(
      std::string const &modelFileName) 
  {
    try
    {
      svt::StDataHdf5 modelMap(modelFileName.c_str());
      modelMap.setExceptionFlag(true);

      _featureGroups.resize(modelMap.getArraySize("featureGroups"));
      modelMap.getArray("featureGroups",
                        _featureGroups.begin(),
                        static_cast<int>(_featureGroups.size()));
      _featureBaseGroup = _featureGroups.front().substr(
          0, _featureGroups.front().rfind("/"));
      _featureBaseGroup = _featureBaseGroup.substr(
          0, _featureBaseGroup.rfind("/"));
      _normalize.resize(_featureGroups.size());
      std::vector<int> normalize(_featureGroups.size());
      modelMap.getArray(
          "featureNormalization",
          normalize.begin(), static_cast<int>(normalize.size()));
      for (size_t i = 0; i < normalize.size(); ++i)
          _normalize[i] = static_cast<NormalizationType>(normalize[i]);
      _featureNames.resize(_featureGroups.size(), NULL);
      _means.resize(_featureGroups.size(), NULL);
      _stddevs.resize(_featureGroups.size(), NULL);
      for (size_t i = 0; i < _featureGroups.size(); ++i) 
      {
        std::string groupName = h5GroupName(_featureGroups[i]);
        _featureNames[i] = new std::vector<std::string>(
            modelMap.getArraySize("featureNames_" + groupName));
        modelMap.getArray("featureNames_" + groupName,
                          _featureNames[i]->begin(),
                          static_cast<int>(_featureNames[i]->size()));
        _means[i] = new std::vector<double>(
            modelMap.getArraySize("featureNames_" + groupName));
        modelMap.getArray("featureMeans_" + groupName,
                          _means[i]->begin(),
                          static_cast<int>(_means[i]->size()));
        _stddevs[i] = new std::vector<double>(
            modelMap.getArraySize("featureNames_" + groupName));
        modelMap.getArray("featureStddevs_" + groupName,
                          _stddevs[i]->begin(),
                          static_cast<int>(_stddevs[i]->size()));
      }
    }
    catch (std::exception &e)
    {
      if (p_progress != NULL) p_progress->abortWithError(
          std::string("Could not load normalization parameters from SVM "
                      "model:") + e.what());
    }
  }

  void Features::trainTwoClassSVM(
      std::vector<svt::BasicFV> &trainVectors,
      std::string const &modelFileName,
      float cost, float gamma) 
  {
    std::cout << "Training two-class SVM on " << trainVectors.size()
              << " training samples" << std::endl;
    
    svt::Model<svt::BasicFV> model;
    svt::TwoClassSVMc<svt::Kernel_RBF> svm;
    svm.kernel().setGamma(gamma);
    svm.setCost(cost); 
    svm.updateKernelCache(
        trainVectors.begin(), trainVectors.end(), svt::DirectAccessor());
    svm.train(trainVectors.begin(), trainVectors.end(), model);
    try
    {
      svt::StDataHdf5 modelMap(modelFileName.c_str(), H5F_ACC_RDWR);
      modelMap.setExceptionFlag(true);
      svm.saveParameters(modelMap);
      model.saveParameters(modelMap);
    }
    catch (std::exception& e) 
    {
      if (p_progress != NULL) p_progress->abortWithError(
          std::string("Could not save SVM model: ") + e.what());
    }
    std::cout << "Training finished" << std::endl;
  }

  void Features::classifyTwoClassSVM(
      std::vector<svt::BasicFV> &testVectors,
      std::string const &modelFileName)
  {
    std::cout << "Classifying " << testVectors.size() << " test samples"
              << std::endl;
    svt::Model<svt::BasicFV> model;
    svt::TwoClassSVMc<svt::Kernel_RBF> svm;

    try
    {
      svt::StDataHdf5 modelMap(modelFileName.c_str());
      modelMap.setExceptionFlag(true);
      model.loadParameters(modelMap);    
      svm.loadParameters(modelMap);
    }
    catch (std::exception &e)
    {
      if (p_progress != NULL) p_progress->abortWithError(
          "Could not load SVM model from '" + modelFileName + "': " + e.what());
      return;
    }
    
    svm.clearKernelCache();
    
    double progressStepPerClassification = 0.0;
    if (p_progress != NULL)
    {
      progressStepPerClassification =
          static_cast<double>(
              p_progress->taskProgressMax() - p_progress->taskProgressMin()) /
          static_cast<double>(testVectors.size());
      p_progress->updateProgressMessage("Classifying...");
    }

    int nClassified = 0;
#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(testVectors.size()); ++i)
    {
      if (p_progress != NULL)
      {
        if (p_progress->isAborted()) continue;
#ifdef _OPENMP
#pragma omp critical
#endif
        {
          nClassified++;
          int progress = static_cast<int>(
              static_cast<double>(p_progress->taskProgressMin()) +
              static_cast<double>(nClassified) * progressStepPerClassification);
          p_progress->updateProgress(progress);
        }
      }
      testVectors[i].setLabel(svm.classify(testVectors[i], model));
    }
    std::cout << "Classification finished" << std::endl;
  }

  void Features::trainMultiClassSVM(
      std::vector<svt::BasicFV> &trainVectors,
      std::string const &modelFileName,
      float cost, float gamma) 
  {
    std::cout << "Training multi-class SVM on " << trainVectors.size()
              << " training samples" << std::endl;
    
    svt::Model_MC_OneVsOne< svt::Model<svt::BasicFV> > model;
    svt::MultiClassSVMOneVsOne<svt::TwoClassSVMc<svt::Kernel_RBF> > svm;
    svm.twoClassSVM().kernel().setGamma(gamma);
    svm.twoClassSVM().setCost(cost); 
    svm.updateKernelCache(
        trainVectors.begin(), trainVectors.end(), svt::DirectAccessor());
    svm.train(trainVectors.begin(), trainVectors.end(), model);
    try
    {
      svt::StDataHdf5 modelMap(modelFileName.c_str(), H5F_ACC_RDWR);
      modelMap.setExceptionFlag(true);
      svm.saveParameters(modelMap);
      model.saveParameters(modelMap);
    }
    catch (std::exception& e) 
    {
      if (p_progress != NULL) p_progress->abortWithError(
          std::string("Could not save SVM model: ") + e.what());
    }
    std::cout << "Training finished" << std::endl;
  }

  void Features::classifyMultiClassSVM(
      std::vector<svt::BasicFV> &testVectors,
      std::string const &modelFileName) 
  {
    std::cout << "Classifying " << testVectors.size() << " test samples"
              << std::endl;
    svt::Model_MC_OneVsOne< svt::Model<svt::BasicFV> > model;
    svt::MultiClassSVMOneVsOne< svt::TwoClassSVMc<svt::Kernel_RBF> > svm;

    try
    {
      svt::StDataHdf5 modelMap(modelFileName.c_str());
      modelMap.setExceptionFlag(true);
      model.loadParameters(modelMap);
      svm.loadParameters(modelMap);
    }
    catch (std::exception &e)
    {
      if (p_progress != NULL) p_progress->abortWithError(
          "Could not load SVM model from '" + modelFileName + "': " + e.what());
      return;
    }

    svm.clearKernelCache();
    
    double progressStepPerClassification = 0.0;
    double progress = 0;
    if (p_progress != NULL)
    {
      progress = p_progress->progress();
      progressStepPerClassification =
          static_cast<double>(
              p_progress->taskProgressMax() - p_progress->taskProgressMin()) /
          static_cast<double>(testVectors.size());
      p_progress->updateProgressMessage("Classifying...");
    }

#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(testVectors.size()); ++i)
    {
      if (p_progress != NULL)
      {
        if (p_progress->isAborted()) continue;
#ifdef _OPENMP
#pragma omp atomic
#endif
        progress += progressStepPerClassification;
        p_progress->updateProgress(static_cast<int>(progress));
      }
      
      testVectors[i].setLabel(svm.classify(testVectors[i], model));
    }
    std::cout << "Classification finished" << std::endl;
  }

  std::string Features::h5GroupName(const std::string& rawGroup)
  {
    std::string res = rawGroup;
    while(res.find("/") != std::string::npos) res[res.find("/")] = '_';
    return res;
  }

}
