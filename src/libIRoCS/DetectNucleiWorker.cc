#include "DetectNucleiWorker.hh"
#include "iRoCSFeatures.hh"

#include <libArrayToolbox/LocalMaximumExtraction.hh>

namespace iRoCS
{

  void detectNuclei(
      atb::Array<double,3> const &data, std::vector<atb::Nucleus> &nuclei,
      std::string const &modelFileName, ptrdiff_t memoryLimit,
      std::string const &cacheFileName, ProgressReporter *pr)
  {
    double sigmaMin = 0.5;
    double sigmaMax = 64.0;
    double sigmaStep = 2.0;
    int bandMax = 5;

    Features features(blitz::TinyVector<double,3>(1.0), pr);

    atb::Array<float,3> classification;

    if (pr != NULL)
    {
      if (!pr->updateProgressMessage(
              "Loading normalization parameters from '" + modelFileName + "'"))
          return;
      pr->updateProgress(0);
    }
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

    if (pr != NULL && !pr->updateProgressMessage("Computing chunk size"))
        return;

    blitz::TinyVector<ptrdiff_t,3> featureShape(
        blitz::TinyVector<double,3>(data.shape()) * data.elementSizeUm() /
        features.elementSizeUm());

    int nFeatures = 0;
    for (double sigma = sigmaMin; sigma <= sigmaMax; sigma *= sigmaStep)
        for (int laplace = 0; laplace <= bandMax / 2; ++laplace)
            for (int band = 0; band <= bandMax - 2 * laplace; ++band)
                nFeatures++;
    nFeatures += 4; // The hough features

    ptrdiff_t nVoxels = blitz::product(featureShape);
    ptrdiff_t memNeeded = nVoxels * nFeatures * sizeof(double);
    ptrdiff_t nChunks = (memoryLimit != 0) ?
        (memNeeded / memoryLimit + ((memNeeded % memoryLimit > 0) ? 1 : 0)) :
        1;
    ptrdiff_t chunkSize = nVoxels / nChunks;
    ptrdiff_t residualFeatures = nVoxels % nChunks;

    std::cout << "Classifying in " << nChunks << " chunk"
              << ((nChunks > 1) ? "s" : "") << " with chunksize "
              << chunkSize << " (" << chunkSize * nFeatures *
        sizeof(double) / 1024 / 1024 << " MB)" << std::endl;

    // Put the remaining features into the first chunk
    std::vector<svt::BasicFV> testVectors(chunkSize + residualFeatures);

    if (pr != NULL && !pr->updateProgressMessage("Initializing test vectors"))
        return;

    for (size_t i = 0; i < testVectors.size(); ++i)
        testVectors[i].resize(nFeatures);

    classification.resize(featureShape);
    classification.setElementSizeUm(features.elementSizeUm());

    if (pr != NULL && pr->isAborted()) return;

    double progressPerChunk = 100.0 / static_cast<double>(nChunks);
    double progressLoadFeatures = progressPerChunk * 0.1;
    double progressStepPerFeatureLoad = progressLoadFeatures / nFeatures;
    double progressClassify = progressPerChunk - progressLoadFeatures;

    ptrdiff_t currentPos = 0;
    for (ptrdiff_t chunk = 0; chunk < nChunks;
         ++chunk, currentPos += testVectors.size())
    {
      if (chunk != 0) testVectors.resize(chunkSize);

      // Compute SD features
      int feaIdx = 0;
      for (double sigma = sigmaMin; sigma <= sigmaMax; sigma *= sigmaStep)
      {
        for (int laplace = 0; laplace <= bandMax / 2; ++laplace)
        {
          int maxBand = bandMax - 2 * laplace;
          for (int band = 0; band <= maxBand; ++band, ++feaIdx)
          {
            atb::Array<double,3> &fea = features.sdFeature(
                data, atb::SDMagFeatureIndex(sigma, laplace, band), maxBand,
                cacheFileName);
            
#ifdef _OPENMP
#pragma omp parallel for
#endif
            for (ptrdiff_t j = 0;
                 j < static_cast<ptrdiff_t>(testVectors.size()); ++j)
                testVectors[j][feaIdx] = fea.data()[currentPos + j];

            int progress = static_cast<int>(
                static_cast<double>(chunk) * progressPerChunk +
                static_cast<double>(feaIdx) * progressStepPerFeatureLoad);
            if (pr != NULL && !pr->updateProgress(progress))
                return;
          }
          for (int band = 1; band <= bandMax - 2 * laplace; ++band)
              features.deleteFeature(
                  atb::SDMagFeatureIndex(sigma, laplace, band));
        }
        for (int laplace = 0; laplace <= bandMax / 2; ++laplace)
            features.deleteFeature(
                atb::SDMagFeatureIndex(sigma, laplace, 0));
        features.deleteFeature(atb::SDMagFeatureIndex(sigma, 0, 0));
      }
    
      // Compute hough features
      for (int i = iRoCS::Features::PositiveMagnitude;
           i <= iRoCS::Features::NegativeRadius; ++i, ++feaIdx)
      {
        atb::Array<double,3> &fea =
            features.houghFeature(data, i, cacheFileName);

#ifdef _OPENMP
#pragma omp parallel for
#endif
        for (ptrdiff_t j = 0; j < static_cast<ptrdiff_t>(testVectors.size());
             ++j)
            testVectors[j][feaIdx] = fea.data()[currentPos + j];

        int progress = static_cast<int>(
            static_cast<double>(chunk) * progressPerChunk +
            static_cast<double>(feaIdx) * progressStepPerFeatureLoad);
        if (pr != NULL &&
            !pr->updateProgress(static_cast<int>(progress))) return;

        features.deleteFeature(i);
      }
    
      if (pr != NULL && !pr->updateProgressMessage("Normalizing features"))
          return;
      features.normalizeFeatures(testVectors);
    
      if (pr != NULL) pr->setTaskProgressRange(
          static_cast<int>(static_cast<double>(chunk) * progressPerChunk +
                           static_cast<double>(progressLoadFeatures)),
          static_cast<int>(static_cast<double>(chunk) * progressPerChunk +
                           static_cast<double>(
                               progressLoadFeatures + progressClassify)));

      if (pr != NULL && !pr->updateProgressMessage("Starting detection"))
          return;
      features.classifyTwoClassSVM(testVectors, modelFileName);
    
#ifdef _OPENMP
#pragma omp parallel for
#endif
      for (ptrdiff_t j = 0; j < static_cast<ptrdiff_t>(testVectors.size()); ++j)
          classification.data()[currentPos + j] =
              static_cast<float>(testVectors[j].getLabel());
    }
  
    if (pr != NULL) pr->updateProgressMessage(
        "Freeing memory used up by features");
    testVectors.clear();
  
    if (cacheFileName != "")
    {
      if (pr != NULL && !pr->updateProgressMessage(
              "Saving decision values to '" + cacheFileName +
              ":/decisionValues'")) return;
      try
      {
        classification.save(cacheFileName, "/decisionValues", 1, pr);
      }
      catch (BlitzH5Error &e)
      {
        std::cerr << "Could not save decision values: " << e.what()
                  << std::endl;
      }
    }
    
    if (pr != NULL && !pr->updateProgressMessage("Extracting local maxima"))
        return;

    std::vector< blitz::TinyVector<ptrdiff_t,3> > lcMax;
    atb::extractLocalMaxima(
        classification, lcMax, 0.0f, atb::SIMPLE_NHOOD, pr);
    std::cout << "  " << lcMax.size() << " local maxima extracted" << std::endl;

    if (pr != NULL && pr->isAborted()) return;

    // Remove overlapping detections
    if (pr != NULL && !pr->updateProgressMessage("Removing overlapping nuclei"))
        return;
    std::vector<atb::Nucleus> ncTmp;
    for (size_t i = 0; i < lcMax.size(); ++i)
    {
      if (pr != NULL && pr->isAborted()) return;
      atb::Nucleus nc;
      nc.setPositionUm(blitz::TinyVector<double,3>(
                           lcMax[i] * classification.elementSizeUm()));
      nc.setRadiusUm(2.0);
      nc.setValue(static_cast<double>(classification(lcMax[i])));
      ncTmp.push_back(nc);
    }
    lcMax.clear();
    std::sort(ncTmp.begin(), ncTmp.end());
    std::reverse(ncTmp.begin(), ncTmp.end());
    std::vector<atb::Nucleus>::const_iterator ncIt2;
    double minDist = 3.0;
    for (size_t i = 0; i < ncTmp.size(); ++i)
    {
      if (pr != NULL && pr->isAborted()) return;
      size_t j = 0;
      for (; j < nuclei.size(); ++j)
      {
        if (blitz::dot(ncTmp[i].positionUm() - nuclei[j].positionUm(),
                       ncTmp[i].positionUm() - nuclei[j].positionUm()) <
            blitz::pow2(minDist)) break;
      }
      if (j == nuclei.size()) nuclei.push_back(ncTmp[i]);
    }
    ncTmp.clear();
    std::cout << "  " << nuclei.size() << " nucleus candidates remaining"
              << std::endl;
    
  }
}
