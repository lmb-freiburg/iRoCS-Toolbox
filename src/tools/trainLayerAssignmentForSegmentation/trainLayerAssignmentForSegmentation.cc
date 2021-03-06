/**************************************************************************
 *
 * Copyright (C) 2015 Kun Liu, Thorsten Falk
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

  CmdArgTypeVector<std::string> inFileNames(
      "<infiles>", "The HDF5 files containing the segmentations and "
      "the iRoCS shell coordinate transforms. Both will only be used if "
      "'--computeFeatures' is given.", CmdArg::isREQ);

  CmdArgType<std::string> datasetName(
      'd', "dataset", "<hdf5 dataset name>",
      "dataset name (HDF5) containing the segmentation labels. This is "
      "required for on-the-fly feature computation.");

  CmdArgType<std::string> sctGroupName(
      's', "shellCoordinateTransformGroup", "<hdf5 group name>",
      "The name of the hdf5 group to read the iRoCS shell coordinate "
      "transform from. This is required for on-the-fly feature computation.");

  CmdArgType<float> thresholdOnVolume(
      't', "thresholdOnVolume", "<double>", "specify the minimum volume "
      "acceptable as a cell (in um_cubic). This is required for on-the-fly "
      "feature computation.");
  thresholdOnVolume.setDefaultValue(20);

  CmdArgType<int> backgroundLabel(
      'b', "backgroundLabel", "<int>", "The label meaning background "
      "in the segmentation dataset. After running segmentCells the background "
      "should be set to label 1 and edges to label 0 (which is in any case "
      "ignored). When passing a negative value, the largest segment will be "
      "used as background segment.");
  backgroundLabel.setDefaultValue(1);

  CmdArgType<std::string> modelFileName(
      'm', "model", "<rf model file name>",
      "The name of the output model file. The generated file is an hdf5 file, "
      "but the ending '.rf.h5' should be used to clarify, that it contains a "
      "random forest model.", CmdArg::isREQ);

  CmdArgSwitch computeFeaturesOnTheFly(
      0, "computeFeatures", "If given the default feature set is computed and "
      "stored to '<featureFile>:<featureGroup> and then classified. If not "
      "given you have to provide pre-computed features using "
      "'--featureDatasets'.");

  CmdArgTypeVector<std::string> featureFileNames(
      0, "featureFiles", "<hdf5 filenames>",
      "A list of input files (HDF5) containing the "
      "features in 2D floating point arrays. If '--computeFeatures' is given "
      "the computed features will be written to these files. If not given, "
      "the input files themselves will be used for feature reading and "
      "writing.");

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

  CmdArgTypeVector<std::string> labelFileNames(
      'y', "labelFiles", "<hdf5 filenames>",
      "A list of input files (HDF5) containing the "
      "labels corresponding to the features. The files must be given in the "
      "same order as the feature files. If they are not given, the input "
      "files will be search for the label dataset.");

  CmdArgType<std::string> labelDsName(
      0, "labelDataset", "<hdf5 dataset name>",
      "The name of the dataset containing the class labels "
      "(an int array). Each label file must contain the dataset");
  labelDsName.setDefaultValue("/classLabels");

  CmdArgType<int> labelRangeLow(
      'l', "lowLabel", "<unsigned integer>",
      "The smallest label used for training", CmdArg::isPOSVALREQ);
  labelRangeLow.setDefaultValue(0);

  CmdArgType<int> labelRangeHigh(
      'h', "highLabel", "<unsigned integer>",
      "The largest label used for training", CmdArg::isPOSVALREQ);
  labelRangeHigh.setDefaultValue(7);

  CmdLine cmd(argv[0], "Random Forest cell shape -> class training");

  try
  {
    cmd.append(&versionArg);
    cmd.append(&licenseArg);

    cmd.append(&inFileNames);
    cmd.append(&datasetName);
    cmd.append(&sctGroupName);
    cmd.append(&thresholdOnVolume);
    cmd.append(&backgroundLabel);
    cmd.append(&modelFileName);
    cmd.append(&computeFeaturesOnTheFly);
    cmd.append(&featureFileNames);
    cmd.append(&featureGroup);
    cmd.append(&featureDsNames);
    cmd.append(&labelFileNames);
    cmd.append(&labelDsName);
    cmd.append(&labelRangeLow);
    cmd.append(&labelRangeHigh);
    cmd.description("Train a random forest on features extracted from "
                    "manually annotated cellular root segmentations");
    /*---------------------------------------------------------------------
     *  Parse command line
     *---------------------------------------------------------------------*/
    ArgvIter arg_iter(--argc, ++argv);
    cmd.parse(arg_iter);

    if (featureFileNames.size() < inFileNames.size())
    {
      for (size_t i = featureFileNames.size(); i < inFileNames.size(); ++i)
          featureFileNames.push_back(inFileNames[i]);
    }

    if (labelFileNames.size() < inFileNames.size())
    {
      for (size_t i = labelFileNames.size(); i < inFileNames.size(); ++i)
          labelFileNames.push_back(inFileNames[i]);
    }

    /*---------------------------------------------------------------------
     *  print given values
     *---------------------------------------------------------------------*/
    std::cout << "infiles = " << std::flush;
    for (size_t i = 0; i < inFileNames.size(); ++i)
        std::cout << "'" << inFileNames[i] << "' " << std::flush;
    std::cout << std::endl;
    std::cout << "featureFiles = " << std::flush;
    for (size_t i = 0; i < featureFileNames.size(); ++i)
        std::cout << "'" << featureFileNames[i] << "' " << std::flush;
    std::cout << std::endl;    
    std::cout << "featureGroup = " << featureGroup.value() << std::endl;
    std::cout << "labelFiles = " << std::flush;
    for (size_t i = 0; i < labelFileNames.size(); ++i)
        std::cout << "'" << labelFileNames[i] << "' " << std::flush;
    std::cout << std::endl;
    std::cout << "labelDataset = " << labelDsName.value() << std::endl;
    std::cout << "lowLabel = " << labelRangeLow.value() << std::endl;
    std::cout << "highLabel = " << labelRangeHigh.value() << std::endl;
    std::cout << "model = " << modelFileName.value() << std::endl;
    if (computeFeaturesOnTheFly.given())
    {
      std::cout << "computeFeatures = yes" << std::endl;
      if (!datasetName.given() || !sctGroupName.given())
          throw CmdLineUsageError()
              << "Mandatory parameter '-d' (--dataset) and 's' "
              << "(--shellCoordinateTransformGroup) missing for on-the-fly "
              << "feature computation. Please provide either pre-computed "
              << "features using '-f' (--featureFiles) and "
              << "'--featureDatasets', or give the dataset containing the "
              << "segmentation and the iRoCS shell coordinate transform to "
              << "compute the features.";
      std::cout << "dataset = " << datasetName.value() << std::endl;
      std::cout << "shellCoordinateTransformGroup = " << sctGroupName.value()
                << std::endl;
      std::cout << "thresholdOnVolume = " << thresholdOnVolume.value()
                << std::endl;
      std::cout << "backgroundLabel = " << backgroundLabel.value() << std::endl;
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

    trainLayerAssignmentToSegmentation(
        inFileNames, datasetName.value(), sctGroupName.value(),
        thresholdOnVolume.value(), modelFileName.value(), featureFileNames,
        featureGroup.value(), featureDsNames, labelFileNames,
        labelDsName.value(), blitz::TinyVector<int,2>(
            labelRangeLow.value(), labelRangeHigh.value()),
        computeFeaturesOnTheFly.given(), backgroundLabel.value(), &pr);
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
