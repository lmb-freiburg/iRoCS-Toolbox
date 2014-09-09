#include "AssignLayersToCellSegmentationWorker.hh"
#include "ComputeCellFeaturesWorker.hh"

#include <libArrayToolbox/algo/helper.hh>
#include <libArrayToolbox/algo/lBlitzRandomForest.hh>

namespace iRoCS
{

  void trainLayerAssignmentToSegmentation(
      std::vector<std::string> const &infiles,
      std::string const &segmentationName, std::string const &sctName,
      double volumeThresholdUm3, std::string const &modelFileName,
      std::vector<std::string> const &featureFileNames,
      std::string const &featureGroup, std::vector<std::string> &featureNames,
      std::vector<std::string> const &labelFileNames,
      std::string const &labelName, blitz::TinyVector<int,2> const &labelRange,
      bool computeFeatures, int backgroundLabel, ProgressReporter *pr)
  {
    if (computeFeatures)
    {
      if (pr != NULL && !pr->updateProgressMessage(
              "Computing standard feature set")) return;
      for (size_t i = 0; i < infiles.size(); ++i)
      {
        atb::Array<int,3> L;
        ShellCoordinateTransform sct;
        if (pr != NULL && !pr->updateProgressMessage(
                "Loading '" + infiles[i] + ":" + segmentationName + "'"))
            return;
        try
        {
          L.load(infiles[i], segmentationName, pr);
        }
        catch (BlitzH5Error &e)
        {
          std::string msg(
              std::string("Could not load segmentation: ") + e.what());
          if (pr != NULL)
          {
            pr->abortWithError(msg);
            return;
          }
          else
          {
            std::cerr << msg << std::endl;
            exit(-1);
          }
        }
        
        if (pr != NULL && !pr->updateProgressMessage(
                "Loading Shell Coordinate Transform from '" + infiles[i] +
                ":" + sctName)) return;
        try
        {
          sct.load(infiles[i], sctName);
        }
        catch (BlitzH5Error &e)
        {
          std::string msg(
              std::string("Could not load iRoCS shell coordinate transform:") +
              e.what());
          if (pr != NULL)
          {
            pr->abortWithError(msg);
            return;
          }
          else
          {
            std::cerr << msg << std::endl;
            exit(-1);
          }
        }
        computeCellFeatures(
            L, sct, volumeThresholdUm3, featureFileNames[i], featureGroup,
            backgroundLabel, pr);
      }
      featureNames.clear();
      featureNames.push_back("normCenters");
      featureNames.push_back("RD");
      featureNames.push_back("blockSize");
      featureNames.push_back("volumeOverBlock");
      featureNames.push_back("convexity");
    }

    if (pr != NULL && !pr->updateProgressMessage("Analyzing features")) return;

    // Get Feature Array extents and check input files
    blitz::TinyVector<atb::BlitzIndexT,2> featureShape(atb::BlitzIndexT(0));
    for (size_t i = 0; i < featureFileNames.size(); ++i)
    {
      atb::BlitzIndexT nSegments = 0;
      try
      {
        atb::BlitzIndexT nFeatures = 0;
        BlitzH5File inFile(featureFileNames[i]);

        // Check valid flags
        blitz::Array<unsigned char,1> vld;
        inFile.readDataset(vld, featureGroup + "/validFlag");
        nSegments = vld.extent(0);
        int nValid = 0;
        for (size_t i = 0; i < vld.size(); ++i) nValid += vld.data()[i];

        // Check features
        for (std::vector<std::string>::const_iterator it =
                 featureNames.begin(); it != featureNames.end(); ++it)
        {
          std::string feaName(featureGroup + "/" + *it);
          std::vector<hsize_t> extents(inFile.getDatasetShape(feaName));
          if (extents.size() > 2)
              throw BlitzH5Error(feaName + " is no valid feature dataset.");
          nFeatures += (extents.size() == 1) ? 1 : extents[1];
          if (static_cast<atb::BlitzIndexT>(extents[0]) != nSegments)
              throw BlitzH5Error(
                  "Incompatible number of segments for feature '" + feaName +
                  "'");
        }
        if (featureShape(1) != 0)
        {
          if (featureShape(1) != nFeatures)
              throw BlitzH5Error("Incompatible number of features");
        }
        else featureShape(1) = nFeatures;
        featureShape(0) += nValid;
      }
      catch (BlitzH5Error & e)
      {
        std::string msg(
            "Error while reading '" + featureFileNames[i] + "': " + e.what());
        if (pr != NULL)
        {
          pr->abortWithError(msg);
          return;
        }
        else
        {
          std::cerr << msg << std::endl;
          exit(-1);
        }
      }
        
      // Check corresponding labels
      try
      {
        BlitzH5File inFile(labelFileNames[i]);
        std::vector<hsize_t> extents(inFile.getDatasetShape(labelName));
        if (extents.size() > 1)
            throw BlitzH5Error(labelName + " is no valid label dataset.");
        if (static_cast<ptrdiff_t>(extents[0]) != nSegments)
            throw BlitzH5Error(
                labelName + " contains the wrong number of labels.");
      }
      catch (BlitzH5Error &e)
      {
        std::string msg(
            "Error while reading '" + labelFileNames[i] + "': " + e.what());
        if (pr != NULL)
        {
          pr->abortWithError(msg);
          return;
        }
        else
        {
          std::cerr << msg << std::endl;
          exit(-1);
        }
      }
    }
      
    if (pr != NULL && !pr->updateProgressMessage(
            "Loading features and labels")) return;
    
    // Load features and labels
    blitz::Array<float,2> features(featureShape);
    blitz::Array<int,1> labels(featureShape(0));
    int segStart = 0;
    for (size_t i = 0; i < featureFileNames.size(); ++i)
    {
      blitz::Array<unsigned char,1> valid;

      try
      {
        BlitzH5File inFile(featureFileNames[i]);

        // Load valid flags
        inFile.readDataset(valid, featureGroup + "/validFlag");

        atb::BlitzIndexT feaStart = 0;
        for (std::vector<std::string>::const_iterator it =
                 featureNames.begin(); it != featureNames.end(); ++it)
        {
          std::string feaName(featureGroup + "/" + *it);
          std::cout << "  Loading '" << featureFileNames[i] << ":" << feaName
                    << "'" << std::endl;
          if (inFile.getDatasetShape(feaName).size() == 1)
          {
            blitz::Array<float,1> fea;
            inFile.readDataset(fea, feaName);
            atb::BlitzIndexT segOffset = 0;
            for (atb::BlitzIndexT segIdx = 0; segIdx < fea.extent(0); ++segIdx)
            {
              if (valid(segIdx) == 1)
              {
                features(segStart + segOffset, feaStart) = fea(segIdx);
                ++segOffset;
              }
            }
            feaStart++;
          }
          else
          {
            blitz::Array<float,2> fea;
            inFile.readDataset(fea, feaName);
            atb::BlitzIndexT segOffset = 0;
            for (atb::BlitzIndexT segIdx = 0; segIdx < fea.extent(0); ++segIdx)
            {
              if (valid(segIdx) == 1)
              {
                for (atb::BlitzIndexT feaIdx = 0; feaIdx < fea.extent(1);
                     ++feaIdx)
                    features(segStart + segOffset, feaStart + feaIdx) =
                        fea(segIdx, feaIdx);
                ++segOffset;
              }
            }
            feaStart += fea.extent(1);
          }
        }
      }
      catch (BlitzH5Error & e)
      {
        std::stringstream msg;
        msg << "Error while reading '" << featureFileNames[i]
            << "': " << e.what();
        if (pr != NULL)
        {
          pr->abortWithError(msg.str());
          return;
        }
        else
        {
          std::cerr << msg.str() << std::endl;
          exit(-1);
        }
      }

      try
      {
        BlitzH5File inFile(labelFileNames[i]);
        std::cout << "  Loading '" << labelFileNames[i] << ":" << labelName
                  << "'" << std::endl;
        blitz::Array<int,1> lbl;
        inFile.readDataset(lbl, labelName);
        atb::BlitzIndexT segOffset = 0;
        for (atb::BlitzIndexT segIdx = 0; segIdx < lbl.extent(0); ++segIdx)
        {
          if (valid(segIdx) == 1)
          {
            labels(segStart + segOffset) = lbl(segIdx);
            ++segOffset;
          }
        }
        segStart += segOffset;
      }
      catch (BlitzH5Error & e)
      {
        std::stringstream msg;
        msg << "Error while reading '" << labelFileNames[i] << "': "
            << e.what();
        if (pr != NULL)
        {
          pr->abortWithError(msg.str());
          return;
        }
        else
        {
          std::cerr << msg.str() << std::endl;
          exit(-1);
        }
      }      
    }
    
    int nTree = 200;
    lRandomForest forest(nTree);
    trainRFSimple(forest, features, labels, 20, labelRange(0), labelRange(1));
    saveRFSimple(forest, modelFileName);

    try
    {
      BlitzH5File outFile(modelFileName, BlitzH5File::WriteOrNew);
      outFile.writeAttribute(nTree, "nTree", "/");
      outFile.writeAttribute(forest._maxLabel, "maxLabel", "/");
      outFile.writeAttribute(featureShape(0), "nTrainingSamples", "/");
      outFile.writeAttribute(featureShape(1), "nFeatures", "/");
      outFile.writeAttribute(volumeThresholdUm3, "volumeThreshold_um3", "/");
      outFile.writeAttribute(labelRange, "labelRange", "/");
      for (size_t i = 0; i < infiles.size(); ++i)
      {
        std::stringstream attName;
        attName << "infile_" << i;
        outFile.writeAttribute(infiles[i], attName.str(), "/");
      }
      outFile.writeAttribute(featureGroup, "featureGroup", "/");
      for (size_t i = 0; i < featureNames.size(); ++i)
      {
        std::stringstream attName;
        attName << "featureName_" << i;
        outFile.writeAttribute(featureNames[i], attName.str(), "/");
      }
    }
    catch (BlitzH5Error &e)
    {
      std::cerr << "Could not write Random forest parameters: " << e.what()
                << std::endl;
      exit(-1);
    }
  }

  void assignLayersToSegmentation(
      atb::Array<int,3> const &L, ShellCoordinateTransform const &sct,
      double volumeThresholdUm3, std::string const &modelFileName,
      std::string const &featureFileName, std::string const &featureGroup,
      std::vector<std::string> &featureNames,
      std::string const &outFileName, std::string const &labelName,
      int backgroundLabel, ProgressReporter *pr)
  {
    if (L.size() != static_cast<size_t>(0))
    {
      // on-the-fly feature computation requested
      if (pr != NULL && !pr->updateProgressMessage(
              "Computing standard feature set")) return;
      computeCellFeatures(
          L, sct, volumeThresholdUm3, featureFileName, featureGroup,
          backgroundLabel, pr);
      featureNames.clear();
      featureNames.push_back("normCenters");
      featureNames.push_back("RD");
      featureNames.push_back("blockSize");
      featureNames.push_back("volumeOverBlock");
      featureNames.push_back("convexity");      
    }

    if (pr != NULL && !pr->updateProgressMessage("Analyzing features")) return;

    // Get Feature Array extents and check input file
    blitz::TinyVector<atb::BlitzIndexT,2> featureShape(atb::BlitzIndexT(0));
    atb::BlitzIndexT nSegments = 0;
    try
    {
      atb::BlitzIndexT nFeatures = 0;
      BlitzH5File inFile(featureFileName);
      for (std::vector<std::string>::const_iterator it =
               featureNames.begin(); it != featureNames.end(); ++it)
      {
        std::string feaName(featureGroup + "/" + *it);
        std::vector<hsize_t> extents(inFile.getDatasetShape(feaName));
        if (extents.size() > 2)
            throw BlitzH5Error(feaName + " is no valid feature dataset.");
        nFeatures += (extents.size() == 1) ? 1 : extents[1];
        if (nSegments != 0)
        {
          if (static_cast<atb::BlitzIndexT>(extents[0]) != nSegments)
              throw BlitzH5Error(
                  "Incompatible number of segments for feature '" + feaName +
                  "'");
        }
        else nSegments = static_cast<atb::BlitzIndexT>(extents[0]);
      }
      featureShape = nSegments, nFeatures;
    }
    catch (BlitzH5Error & e)
    {
      std::stringstream msg;
      msg << "Error while reading '" << featureFileName << "': " << e.what();
      if (pr != NULL)
      {
        pr->abortWithError(msg.str());
        return;
      }
      else
      {
        std::cerr << msg.str() << std::endl;
        exit(-1);
      }
    }
    std::cout << "  Found " << featureShape(0) << " segments, with "
              << featureShape(1) << " features each." << std::endl;
    
    if (pr != NULL && !pr->updateProgressMessage("Loading features")) return;

    // Load features
    blitz::Array<float,2> features(featureShape);
    try
    {
      BlitzH5File inFile(featureFileName);
      atb::BlitzIndexT feaStart = 0;
      for (std::vector<std::string>::const_iterator it =
               featureNames.begin(); it != featureNames.end(); ++it)
      {
        std::string feaName(featureGroup + "/" + *it);
        std::cout << "  Loading '" << featureFileName << ":" << feaName << "'"
                  << std::endl;
        if (inFile.getDatasetShape(feaName).size() == 1)
        {
          blitz::Array<float,1> fea;
          inFile.readDataset(fea, feaName);
          for (atb::BlitzIndexT segIdx = 0; segIdx < fea.extent(0); ++segIdx)
              features(segIdx, feaStart) = fea(segIdx);
          feaStart++;
        }
        else
        {
          blitz::Array<float,2> fea;
          inFile.readDataset(fea, feaName);
          for (atb::BlitzIndexT segIdx = 0; segIdx < fea.extent(0); ++segIdx)
              for (atb::BlitzIndexT feaIdx = 0; feaIdx < fea.extent(1);
                   ++feaIdx)
                  features(segIdx, feaStart + feaIdx) = fea(segIdx, feaIdx);
          feaStart += fea.extent(1);
        }
      }
    }
    catch (BlitzH5Error & e)
    {
      std::stringstream msg;
      msg << "Error while reading '" << featureFileName << "': " << e.what();
      if (pr != NULL)
      {
        pr->abortWithError(msg.str());
        return;
      }
      else
      {
        std::cerr << msg.str() << std::endl;
        exit(-1);
      }
    }
    
    if (pr != NULL && !pr->updateProgressMessage(
            "Loading '" + featureFileName + ":/validFlag'")) return;

    blitz::Array<unsigned char,1> valid;
    try
    {
      BlitzH5File inFile(featureFileName);
      inFile.readDataset(valid, featureGroup + "/validFlag");
    }
    catch (BlitzH5Error & e)
    {
      std::stringstream msg;
      msg << "Error while reading 'validFlag': " << e.what();
      if (pr != NULL)
      {
        pr->abortWithError(msg.str());
        return;
      }
      else
      {
        std::cerr << msg.str() << std::endl;
        exit(-1);
      }
    }    

    if (pr != NULL && !pr->updateProgressMessage(
            "Loading Random Forest model from '" + modelFileName + "'")) return;

    int nTree;
    int maxLabel;
    try
    {
      BlitzH5File inFile(modelFileName);
      inFile.readAttribute(nTree, "nTree", "/");
      inFile.readAttribute(maxLabel, "maxLabel", "/");
    }
    catch (BlitzH5Error &e)
    {
      std::stringstream msg;
      msg << "Could not read random forest parameters: " << e.what()
                << std::endl;
      if (pr != NULL)
      {
        pr->abortWithError(msg.str());
        return;
      }
      else
      {
        std::cerr << msg.str() << std::endl;
        exit(-1);
      }
    }
    lRandomForest forest(nTree);
    loadRFSimple(forest, modelFileName, nTree);
    std::cout << "  Loaded random forest with maxLabel " << forest._maxLabel
              << " and " << forest._nTree << " trees" << std::endl;

    //predict
    blitz::Array<int,1> label(features.extent(0));
    blitz::Array<float,2> prob(features.extent(0), maxLabel + 1);
    prob = -1;
    blitz::Array<float,1> outlier(features.extent(0));
    outlier = -1;
    ptrdiff_t m = features.extent(0);
    ptrdiff_t n = features.extent(1);
#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (atb::BlitzIndexT i = 0; i < m; ++i)
    {
      if (valid(i) != 0)
      {
        label(i) = forest.predict(
            features.data() + n * i, prob.data() + (maxLabel + 1) * i);
        outlier(i) = forest.outlier(features.data() + n * i, label(i));
      }
      else
      {
        label(i) = 0;
        prob(i, 0) = 1.0f;
        for (int label = 1; label < maxLabel + 1; ++label)
            prob(i, label) = 0.0f;
        outlier(i) = 0.0f;
      }
    }

    try
    {
      BlitzH5File outFile(outFileName, BlitzH5File::WriteOrNew);
      outFile.writeDataset(label, labelName);
      // outFile.writeDataset(prob, "/Prob");
      // outFile.writeDataset(outlier, "/Outlier");      
    }
    catch (BlitzH5Error &e)
    {
      std::stringstream msg;
      msg << "Error while writing classification result: " << e.what();
      if (pr != NULL)
      {
        pr->abortWithError(msg.str());
        return;
      }
      else
      {
        std::cerr << msg.str() << std::endl;
        exit(-1);
      }
    }  
  }

}
