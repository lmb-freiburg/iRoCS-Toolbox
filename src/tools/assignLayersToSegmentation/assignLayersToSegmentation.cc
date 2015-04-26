/**************************************************************************
 *
 * Copyright (C) 2014 Kun Liu, Thorsten Falk
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

#include <iostream>
#include <set>

#include <libcmdline/CmdLine.hh>
#include <libcmdline/ArgvIter.hh>

#include <libProgressReporter/ProgressReporterStream.hh>

#include <libBlitzHdf5/BlitzHdf5Light.hh>

#include <libIRoCS/AssignLayersToCellSegmentationWorker.hh>

class CmdLineVersionError: public CmdLineError {};
class CmdLineLicenseError: public CmdLineError {};

int main(int argc, char **argv)
{
  /*---------------------------------------------------------------------
   *  Specify command line arguments and descriptions
   *---------------------------------------------------------------------*/

  CmdArgThrow<CmdLineVersionError> versionArg(
      0, "version", "Display version information.");
  CmdArgThrow<CmdLineLicenseError> licenseArg(
      0, "license", "Display licensing information.");

  CmdArgType<std::string> inFileName(
      "<infile>", "The HDF5 file containing the segmentation and "
      "the iRoCS shell coordinate transform. Both will only be used if "
      "'--computeFeatures' is given.", CmdArg::isREQ);

  CmdArgType<std::string> datasetName(
      'd', "dataset", "<hdf5 dataset name>",
      "dataset name (HDF5) containing the segmentation labels. This is only "
      "required if '--computeFeatures' is also given.");

  CmdArgType<std::string> sctGroupName(
      's', "shellCoordinateTransformGroup", "<hdf5 group name>",
      "The name of the hdf5 group to read the iRoCS shell coordinate "
      "transform from. This is only required if '--computeFeatures' is also "
      "given.");

  CmdArgType<float> thresholdOnVolume(
      't', "thresholdOnVolume", "<double>", "specify the minimum volume "
      "acceptable as a cell (in um_cubic). This is only required if "
      "'--computeFeatures' is also given.");
  thresholdOnVolume.setDefaultValue(20);

  CmdArgType<int> backgroundLabel(
      'b', "backgroundLabel", "<int>", "The label meaning background "
      "in the segmentation dataset. After running segmentCells the background "
      "should be set to label 1 and edges to label 0 (which is in any case "
      "ignored). When passing a negative value, the largest segment will be "
      "used as background segment.");
  backgroundLabel.setDefaultValue(1);

  CmdArgType<std::string> outFileName(
      'o', "outfile", "<hdf5 filename>",
      "The output hdf5 file to store the classification result to.");
  outFileName.setDefaultValue("<infile>");

  CmdArgType<std::string> labelDsName(
      0, "labelDataset", "<hdf5 dataset>",
      "The name of the dataset the predicted class labels are written to.");
  labelDsName.setDefaultValue("/predicted_label");

  CmdArgType<std::string> modelFileName(
      'm', "model", "<rf model file name>",
      "hdf5 file containing the trained RF model", CmdArg::isREQ);

  CmdArgSwitch computeFeaturesOnTheFly(
      0, "computeFeatures", "If given the default feature set is computed and "
      "stored to '<featureFile>:<featureGroup> and then classified. If not "
      "given you have to provide pre-computed features using "
      "'--featureDatasets'.");

  CmdArgType<std::string> featureFileName(
      0, "featureFile", "<hdf5 filename>",
      "input file (HDF5) for feature reading/writing. If '--computeFeatures' "
      "is given, the features will be stored to <featureFile>:<featureGroup>, "
      "otherwise this file must contain pre-computed features with the names "
      "given in '--featureDatasets'.");
  featureFileName.setDefaultValue("<infile>");

  CmdArgType<std::string> featureGroup(
      'f', "featureGroup", "<hdf5 group>", "The HDF5 group to store the "
      "computed cell features to or read pre-computed features from. If the "
      "features were pre-computed a dataset with name <featureGroup>/validFlag "
      "must exist, that is 1 if the segment is valid, and 0 otherwise.");
  featureGroup.setDefaultValue("/features_SCT");

  CmdArgTypeVector<std::string> featureDsNames(
      0, "featureDatasets", "<list of hdf5 datasets>",
      "The names of the datasets containing the features. The hdf5 group "
      "specified by '-f' (--featureGroup) will be prepended making up the full "
      "feature dataset path. The features will be appended to each other in "
      "the order specified. Make sure to use the same order that was used for "
      "training. This is only used if pre-computed features are used. If "
      "'--computeFeatures' is given, the '--featureDatasets' parameter is "
      "ignored.");

  CmdLine cmd(argv[0], "Apply random forest");

  try
  {
    cmd.append(&versionArg);
    cmd.append(&licenseArg);

    cmd.append(&inFileName);
    cmd.append(&outFileName);
    cmd.append(&labelDsName);
    cmd.append(&modelFileName);
    cmd.append(&featureFileName);
    cmd.append(&featureGroup);

    cmd.append(&computeFeaturesOnTheFly);

    cmd.append(&datasetName);
    cmd.append(&sctGroupName);
    cmd.append(&thresholdOnVolume);
    cmd.append(&backgroundLabel);

    cmd.append(&featureDsNames);

    cmd.description(
        "Classify the feature vectors of the segments using a trained "
        "random forest.");

    /*---------------------------------------------------------------------
     *  Parse command line
     *---------------------------------------------------------------------*/
    ArgvIter arg_iter(--argc, ++argv);
    cmd.parse(arg_iter);

    /*---------------------------------------------------------------------
     *  print given values
     *---------------------------------------------------------------------*/
    std::string ofName =
        (outFileName.value() == "" || outFileName.value() == "<infile>") ?
        inFileName.value() : outFileName.value();
    std::string feaName =
        (featureFileName.value() == "" ||
         featureFileName.value() == "<infile>") ?
        inFileName.value() : featureFileName.value();
    std::cout << "infile = " << inFileName.value() << std::endl;
    std::cout << "outfile = " << ofName << std::endl;
    std::cout << "labelDataset = " << labelDsName.value() << std::endl;
    std::cout << "model = " << modelFileName.value() << std::endl;
    std::cout << "featureFile = " << featureFileName.value() << std::endl;
    std::cout << "featureGroup = " << featureGroup.value() << std::endl;
    if (computeFeaturesOnTheFly.given()) 
    {
      std::cout << "computeFeatures = yes" << std::endl;
      if (!datasetName.given() || !sctGroupName.given())
          throw CmdLineUsageError()
              << "Mandatory parameter '-d' (--dataset) and 's' "
              << "(--shellCoordinateTransformGroup) missing for on-the-fly "
              << "feature computation. Please provide either pre-computed "
              << "features using '-f' (--featureFile) and '--featureDatasets', "
              << "or give the dataset containing the segmentation and the "
              << "iRoCS shell coordinate transform to compute the features.";
      std::cout << "dataset = " << datasetName.value() << std::endl;
      std::cout << "shellCoordinateTransformGroup = " << sctGroupName.value()
                << std::endl;
      std::cout << "thresholdOnVolume = " << thresholdOnVolume.value()
                << std::endl;
      std::cout << "backgroundLabel = " << backgroundLabel.value()
                << std::endl;
    }
    else
    {
      std::cout << "computeFeatures = no" << std::endl;
      if (featureDsNames.size() == 0)
          throw CmdLineUsageError()
              << "Mandatory parameter '-f' (--featureDatasets) missing. Please "
              << "provide either pre-computed features using '-f' "
              << "(--featureFile) and '--featureDatasets', or give the "
              << "dataset containing the segmentation and the iRoCS shell "
              << "coordinate transform to compute the features.";
      std::cout << "featureDatasets = " << std::flush;
      for (size_t i = 0; i < featureDsNames.size(); ++i)
          std::cout << "'" << featureDsNames[i] << "' " << std::flush;
      std::cout << std::endl;
    }

    iRoCS::ProgressReporterStream pr(std::cout, 0, 0, 100, "\r   ");
    pr.setTaskProgressMin(0);
    pr.setTaskProgressMax(100);

    atb::Array<int,3> L;
    ShellCoordinateTransform sct;
    if (computeFeaturesOnTheFly.given())
    {
      pr.updateProgressMessage(
          "Loading '" + inFileName.value() + ":" + datasetName.value() + "'");
      try
      {
        L.load(inFileName.value(), datasetName.value(), &pr);
      }
      catch (BlitzH5Error &e)
      {
        std::cerr << "Could not load '" << inFileName.value() << ":"
                  << datasetName.value() << "': " << e.what() << std::endl;
        exit(-1);
      }

      pr.updateProgressMessage(
          "Loading Shell Coordinate Transform from '" + inFileName.value() +
          ":" + sctGroupName.value());
      try
      {
        sct.load(inFileName.value(), sctGroupName.value());
      }
      catch (BlitzH5Error &e)
      {
        std::cerr << "Could not load '" << inFileName.value() << ":"
                  << sctGroupName.value() << "': " << e.what() << std::endl;
        exit(-1);
      }
    }

    assignLayersToSegmentation(
        L, sct, thresholdOnVolume.value(), modelFileName.value(),
        featureFileName.value(), featureGroup.value(), featureDsNames,
        ofName, labelDsName.value(), backgroundLabel.value(), &pr);
  }
  catch (CmdLineUsageError &e)
  {
    cmd.usage();
    exit(0);
  }
  catch (CmdLineVersionError e)
  {
    std::cout << PACKAGE_STRING << std::endl;
    exit(0);
  }
  catch (CmdLineLicenseError e)
  {
    std::cout << PACKAGE_STRING << std::endl << std::endl
              << "URL: " << PACKAGE_URL << std::endl << std::endl
              << "Copyright (C) 2012-2015 Kun Liu, Thorsten Falk ("
              << PACKAGE_BUGREPORT << ")" << std::endl << std::endl
              << "Address:" << std::endl
              << "   Image Analysis Lab" << std::endl
              << "   Albert-Ludwigs-Universitaet" << std::endl
              << "   Georges-Koehler-Allee Geb. 52" << std::endl
              << "   79110 Freiburg" << std::endl
              << "   Germany" << std::endl << std::endl
              << "This program is free software: you can redistribute it and/or"
              << std::endl
              << "modify it under the terms of the GNU General Public License"
              << std::endl
              << "Version 3 as published by the Free Software Foundation."
              << std::endl << std::endl
              << "This program is distributed in the hope that it will be "
              << "useful," << std::endl
              << "but WITHOUT ANY WARRANTY; without even the implied warranty "
              << "of " << std::endl
              << "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the "
              << std::endl
              << "GNU General Public License for more details."
              << std::endl << std::endl
              << "You should have received a copy of the GNU General Public "
              << "License" << std::endl
              << "along with this program. If not, see " << std::endl
              << "<http://www.gnu.org/licenses/>." << std::endl;
    exit(0);
  }
  catch (CmdLineUsageHTMLError &e)
  {
    cmd.usageHTML(std::cout);
    exit(0);
  }
  catch (CmdLineUsageXMLError &e)
  {
    cmd.usageXML(std::cout);
    exit(0);
  }
  catch (CmdLineSyntaxError &e)
  {
    cmd.error() << e.str() << std::endl;
    cmd.usage(std::cerr);
    exit(-1);
  }
  return 0;
}
