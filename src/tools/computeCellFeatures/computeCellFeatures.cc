//============================================================================
// Name        : computeCellFeatures.cc
// Author      : Kun Liu / Thorsten Schmidt
// Version     : 1.0
// Copyright   : (C) 2014 University of Freiburg
// Description : Compute cell shape features of a cellular Arabidopsis root
//               segmentation with attached iRoCS Shell coordinate transform
// License     : GNU General Public License (GPL) version 3
//============================================================================

#include <libcmdline/CmdLine.hh>
#include <libcmdline/ArgvIter.hh>

#include <libProgressReporter/ProgressReporterStream.hh>

#include <libBlitzHdf5/BlitzHdf5Light.hh>

#include <libIRoCS/ComputeCellFeaturesWorker.hh>

int main(int argc, char *argv[])
{
  /*---------------------------------------------------------------------
   *  Specify command line arguments and descriptions
   *---------------------------------------------------------------------*/

  CmdArgType<std::string> inFileName(
      "<infile>", "input file (HDF5) containing the segmentation dataset and "
      "iRoCS group", CmdArg::isREQ);

  CmdArgType<std::string> datasetName(
      'd', "dataset", "<hdf5 dataset name>",
      "dataset name (HDF5) containing the segmentation labels", CmdArg::isREQ);

  CmdArgType<std::string> sctGroupName(
      's', "shellCoordinateTransformGroup", "<hdf5 group name>",
      "The name of the hdf5 group to read the iRoCS shell coordinate "
      "transform from.", CmdArg::isREQ);

  CmdArgType<std::string> outFileName(
      'o', "outfile", "<hdf5 file name>", "output file name.");
  outFileName.setDefaultValue("<infile>");

  CmdArgType<std::string> featureGroup(
      'f', "featureGroup", "<hdf5 group>", "The HDF5 group o store the "
      "computed cell features to.");
  featureGroup.setDefaultValue("/features_SCT");

  CmdArgType<float> thresholdOnVolume(
      't', "thresholdOnVolume", "<double>", "specify the minimum volume "
      "acceptable as a cell (in um_cubic)");
  thresholdOnVolume.setDefaultValue(20);

  CmdArgType<int> backgroundLabel(
      'b', "backgroundLabel", "<int>", "The label meaning background "
      "in the segmentation dataset. After running segmentCells the background "
      "should be set to label 1 and edges to label 0 (which is in any case "
      "ignored). When passing a negative value, the largest segment will be "
      "used as background segment.");
  backgroundLabel.setDefaultValue(1);

  CmdArgSwitch saveMetaData(
      0, "saveIntermediateResults",
      "store intermediate data like eigen-value of hessian and seed points.");

  CmdLine cmd(argv[0], "Compute cell shape features");

  try
  {
    cmd.append(&inFileName);
    cmd.append(&datasetName);
    cmd.append(&sctGroupName);
    cmd.append(&outFileName);
    cmd.append(&featureGroup);
    cmd.append(&thresholdOnVolume);
    cmd.append(&backgroundLabel);
    cmd.append(&saveMetaData);
    cmd.description(
        "Compute features for each segmented cell, including iRoCS position, "
        "radial profile features and convexity.");
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

    std::cout << "saveMetaData = " << saveMetaData.toggled() << std::endl;
    std::cout << "thresholdOnVolume = " << thresholdOnVolume.value()
              << std::endl;
    std::cout << "backgroundLabel = " << backgroundLabel.value() << std::endl;
    std::cout << "datasetName = " << datasetName.value() << std::endl;
    std::cout << "sctGroup = " << sctGroupName.value() << std::endl;
    std::cout << "outFileName = " << ofName << std::endl;
    std::cout << "inFileName = " << inFileName.value() << std::endl;

    iRoCS::ProgressReporterStream pr(std::cout, 0, 0, 100, "\r   ");
    pr.setTaskProgressMin(0);
    pr.setTaskProgressMax(100);

    atb::Array<int,3> L;
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
        "Loading Shell Coordinate Transform from '" + inFileName.value() + ":" +
        sctGroupName.value());
    ShellCoordinateTransform sct;
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

    iRoCS::computeCellFeatures(
        L, sct, thresholdOnVolume.value(), ofName, featureGroup.value(),
        backgroundLabel.value(), &pr);
  }
  catch (CmdLineUsageError& e)
  {
    cmd.usage();
    exit(0);
  }
  catch (CmdLineUsageHTMLError& e)
  {
    cmd.usageHTML(std::cout);
    exit(0);
  }
  catch (CmdLineUsageXMLError& e)
  {
    cmd.usageXML(std::cout);
    exit(0);
  }
  catch (CmdLineSyntaxError& e)
  {
    cmd.error() << e.str() << std::endl;
    cmd.usage(std::cerr);
    exit(-1);
  }

  return 0;
}
