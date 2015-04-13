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

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#include <libcmdline/CmdLine.hh>
#include <libcmdline/ArgvIter.hh>

#include <libIRoCS/DetectSpheresWorker.hh>

#include <libProgressReporter/ProgressReporterStream.hh>

int main(int argc, char **argv)
{
  CmdArgType<std::string> inFileName(
      "<hdf5 file>",
      "The name of the hdf5 file containing the dataset to detect spheres in",
      CmdArg::isREQ);
  CmdArgType<std::string> datasetName(
      'd', "dataset", "<hdf5 dataset>", "The dataset containing spherical "
      "structures to detect", CmdArg::isREQ);
  CmdArgType<std::string> annotationName(
      's', "spheres", "<hdf5 group>", "The group to store the detected "
      "spheres to");
  annotationName.setDefaultValue("/annotation/detector");
  CmdArgType<std::string> responseDataset(
      0, "responseDataset", "<hdf5 dataset name>",
      "The dataset to store the hough response to.");
  responseDataset.setDefaultValue("");
  CmdArgType<std::string> radiusUmDataset(
      0, "radiusDataset", "<hdf5 dataset name>",
      "The dataset to store the hough radii in micrometers to.");
  radiusUmDataset.setDefaultValue("");
  CmdArgType<double> rMinUm(
      0, "rMin", "<double>", "The minimum search radius in micrometers",
      CmdArg::isREQ);
  CmdArgType<double> rMaxUm(
      0, "rMax", "<double>", "The maximum search radius in micrometers",
      CmdArg::isREQ);
  CmdArgType<double> rStepUm(
      0, "rStep", "<double>", "The search radius step in micrometers",
      CmdArg::isREQ);
  CmdArgType<double> preSmoothingSigmaUm(
      0, "preSmoothingSigma", "<double>",
      "The standard deviation of the pre-smoothing filter in micrometers. "
      "This filter regularizaes the gradient computation.");
  preSmoothingSigmaUm.setDefaultValue(0.0);
  CmdArgType<double> postSmoothingSigmaUm(
      0, "postSmoothingSigma", "<double>",
      "The standard deviation of the post-smoothing filter in micrometers. "
      "This filter collects the responses in the filter area.");
  postSmoothingSigmaUm.setDefaultValue(0.0);
  CmdArgType<double> minMagnitude(
      0, "minMagnitude", "<double>",
      "Only gradients with magnitude above this threshold may cast votes. The "
      "given value is relative to the maximum gradient magnitude occuring in "
      "the dataset after pre-smoothing. Only values in the [0,1] range are "
      "sensible. 0: all gradients may vote, 1: no gradient may vote.");
  minMagnitude.setDefaultValue(0.0);
  CmdArgSwitch invertGradients(
      'i', "invertGradients", "If this flag is given the gradients vote in "
      "oppsite direction, leading to the detection of dark spheres on bright "
      "background.");
  CmdArgType<double> gamma(
      'g', "gamma", "<double>",
      "Prior to processing a gamma correction with the given exponent is "
      "applied. I(x)' = I(x)^(1/gamma).");
  gamma.setDefaultValue(1.0);
  CmdArgType<std::string> outFileName(
      'o', "outfile", "<hdf5 file>", "If given, the annotation will be written "
      "to a file of the given name. If omitted the annotations will be written "
      "to the input file");

  CmdLine cmd(argv[0], "Sphere detector");
  cmd.description("Detect spheres in an hdf5 dataset");
  
  try
  {
    cmd.append(&inFileName);
    cmd.append(&datasetName);
    cmd.append(&annotationName);
    cmd.append(&responseDataset);
    cmd.append(&radiusUmDataset);
    cmd.append(&rMinUm);
    cmd.append(&rMaxUm);
    cmd.append(&rStepUm);
    cmd.append(&preSmoothingSigmaUm);
    cmd.append(&postSmoothingSigmaUm);
    cmd.append(&minMagnitude);
    cmd.append(&invertGradients);
    cmd.append(&gamma);
    cmd.append(&outFileName);
    
    ArgvIter argvIter(--argc, ++argv);
    cmd.parse(argvIter);
    
    iRoCS::ProgressReporterStream pr(std::cout, 0, 0, 100, "\r ");
    pr.setTaskProgressMin(0);
    pr.setTaskProgressMax(100);
    
    /*---------------------------------------------------------------------
     *  Load data channel
     *---------------------------------------------------------------------*/
    atb::Array<float,3> data;
    try
    {
      pr.updateProgressMessage(
          "Loading '" + inFileName.value() + ":" + datasetName.value() + "'");
      data.load(inFileName.value(), datasetName.value(), &pr);
    }
    catch(BlitzH5Error &e)
    {
      std::cerr << "Error while loading data channel: " << e.what()
                << std::endl;
      exit(-2);
    }

    /*---------------------------------------------------------------------
     *  Create annotation channel
     *---------------------------------------------------------------------*/ 
    std::vector<atb::Nucleus> spheres;

    /*---------------------------------------------------------------------
     *  Create hough accumulator
     *---------------------------------------------------------------------*/
    atb::Array<float,3> response;
    atb::Array<float,3> radiusUm;
        
    /*---------------------------------------------------------------------
     *  Run detector
     *---------------------------------------------------------------------*/
    pr.updateProgressMessage("Hough Transform");
    iRoCS::detectSpheres(
        data, spheres, response, radiusUm,
        blitz::TinyVector<double,2>(rMinUm.value(), rMaxUm.value()),
        rStepUm.value(), preSmoothingSigmaUm.value(),
        postSmoothingSigmaUm.value(), minMagnitude.value(),
        invertGradients.given(), gamma.value(), &pr);
    if (pr.isAborted()) return -1;
    
    /*---------------------------------------------------------------------
     *  Separate the dataset transformation
     *---------------------------------------------------------------------*/
    for (size_t i = 0; i < spheres.size(); ++i)
        spheres[i].setPositionUm(
            atb::homogeneousToEuclidean(
                data.transformation() * atb::euclideanToHomogeneous(
                    spheres[i].positionUm())));

    /*---------------------------------------------------------------------
     *  Save Annotation channel
     *---------------------------------------------------------------------*/
    std::string ofName = (outFileName.given()) ? outFileName.value() :
        inFileName.value();
    pr.updateProgressMessage(
        "Saving '" + ofName + ":" + annotationName.value() + "'");
    atb::Nucleus::saveList(spheres, ofName, annotationName.value());
    try
    {
      BlitzH5File outFile(ofName, BlitzH5File::Write);
      outFile.writeAttribute(
          data.transformation(), "transformation", annotationName.value());
    }
    catch (BlitzH5Error &e)
    {
      std::cerr << "Could not save annotation channel transformation: "
                << e.what() << std::endl;
      exit(-1);
    }

    /*---------------------------------------------------------------------
     *  If requested also save the hough accumulators
     *---------------------------------------------------------------------*/
    if (responseDataset.value() != "")
    {
      pr.updateProgressMessage(
          "Saving '" + ofName + ":" + responseDataset.value() + "'");
      try
      {
        response.save(ofName, responseDataset.value(), 3, &pr);
      }
      catch (BlitzH5Error &e)
      {
        std::cerr << "Could not save hough response dataset: " << e.what()
                  << std::endl;
        exit(-1);
      }
    }
    
    if (radiusUmDataset.value() != "")
    {
      pr.updateProgressMessage(
          "Saving '" + ofName + ":" + radiusUmDataset.value() + "'");
      try
      {
        radiusUm.save(ofName, radiusUmDataset.value(), 3, &pr);
      }
      catch (BlitzH5Error &e)
      {
        std::cerr << "Could not save hough radius dataset: " << e.what()
                  << std::endl;
        exit(-1);
      }
    }
  }
  catch (CmdLineUsageError &e)
  {
    cmd.usage();
    exit(-1);
  }
  catch (CmdLineUsageHTMLError &e)
  {
    cmd.usageHTML(std::cout);
    exit(-1);
  }
  catch (CmdLineUsageXMLError &e)
  {
    cmd.usageXML(std::cout);
    exit(-1);
  }
  catch (CmdLineSyntaxError &e)
  {
    cmd.error() << e.str() << std::endl;
    cmd.usage(std::cerr);
    exit(-2);
  }    
}
