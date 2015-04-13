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

//============================================================================
// Name        : attachIRoCSToCellSegmentation.cc
// Author      : Kun Liu / Thorsten Schmidt
// Version     : 1.0
// Copyright   : (C) 2014 Computer Vision Lab, University of Freiburg, Germany
// Description : Command line tools to attach iRoCS to a given cellular root
//               segmentation
//============================================================================

#include <iostream>

#include <libcmdline/CmdLine.hh>
#include <libcmdline/ArgvIter.hh>

#include <libProgressReporter/ProgressReporterStream.hh>

#include <libBlitzHdf5/BlitzHdf5Light.hh>

#include <libArrayToolbox/Normalization.hh>
#include <libArrayToolbox/algo/lParallel.hh>

#include <libIRoCS/AttachIRoCSSCTToCellSegmentationWorker.hh>

int main(int argc, char *argv[])
{
  /*---------------------------------------------------------------------
   *  Specify command line arguments and descriptions
   *---------------------------------------------------------------------*/

  CmdArgType<std::string> inFileName(
      "<infile>", "Name of the file containing the segmentation dataset.",
      CmdArg::isREQ);
  CmdArgType<std::string> labelDatasetName(
      'd', "dataset", "<hdf5 dataset name>", "The name of the segmentation "
      "dataset", CmdArg::isREQ);
  CmdArgType<std::string> outFileName(
      'o', "outfile", "<hdf5 file name>", "output file name");
  outFileName.setDefaultValue("<infile>");
  CmdArgType<std::string> sctGroupName(
      's', "shellCoordinateTransformGroup", "<hdf5 group name>",
      "The name of the hdf5 group to write the resulting shell coordinate "
      "transform to", CmdArg::isREQ);
  CmdArgType<int> backgroundLabel(
      'b', "backgroundLabel", "<int>", "The label meaning background "
      "in the segmentation dataset. After running segmentCells the background "
      "should be set to label 1 and edges to label 0 (which is in any case "
      "ignored).");
  backgroundLabel.setDefaultValue(1);
  CmdArgType<double> segmentLengthUm(
      'l', "segmentLengthUm", "<double>", "The segment length in micrometers "
      "of the resulting axis");
  segmentLengthUm.setDefaultValue(10);
  CmdArgType<double> marginUm(
      'm', "margin", "<double>", "Margin in micrometers");
  marginUm.setDefaultValue(30);
  CmdArgType<int> downsampleRatio(
      'r', "downsampleRatio", "<int>", "Point downsample ratio (Only every "
      "<downsampleRatio>s point is used for iRoCS fitting.");
  downsampleRatio.setDefaultValue(4);
  CmdArgTypeFixedVector<double> qcPositionUm(
      0, "qcPositionUm", "<double> <double> <double>",
      "the position of QC in the data, or other position for origin (in um "
      "(Z Y X)). If the QC position was annotated using labelling "
      "you should instead use the '-q (--qcGroup)' parameter and pass it the "
      "hdf5 group containing the QC annotation channel. This parameter "
      "overrides the '--qcGroup' parameter if given.", 3);
  CmdArgType<std::string> qcGroup(
      'q', "qcGroup", "<hdf5 group name>", "The name of the hdf5 group "
      "containing the QC annotation channel. The position of the first maker "
      "of this channel will be used as QC position.");
  CmdArgTypeFixedVector<double> xdir(
      'x', "xDirection", "<double> <double> <double>",
      "vector for the main axis direction (in um (Z Y X)). If not given it "
      "will be determined automatically as the main axis of the PCA of the "
      "union of the foreground regions.", 3);
  CmdArgType<std::string> debugFileName(
      0, "debugfile", "<hdf5 file name>", "If given, debug information will "
      "be written to a file with the given name.");
  CmdArgSwitch computeVisualization(
      'v', "computeVisualization", "Generate a dense visualization of the "
      "fitted shell coordinate transform for debugging purposes. This "
      "requires dense evaluation of the coordinates and is very slow. The "
      "visualization requires the arguments <debugfile>, <imagefile> and "
      "<imageDataset> and will be written to <debugfile>:/show.");
  CmdArgType<std::string> imageFileName(
      0, "imagefile", "<hdf5 file name>", "The name of the file containing the "
      "dataset to put in the background of the generated overlay. This is only "
      "used in conjunction with '-v' and then required.");
  imageFileName.setDefaultValue("");
  CmdArgType<std::string> imageDatasetName(
      0, "imageDataset", "<hdf5 dataset name>", "The name of the dataset to "
      "use as background of the generated overlay. This is only used in "
      "conjunction with '-v' and then required.");
  imageDatasetName.setDefaultValue("");

  CmdLine cmd(argv[0], "Attach iRoCS to a cell segmentation");

  try
  {
    cmd.append(&inFileName);
    cmd.append(&labelDatasetName);
    cmd.append(&outFileName);
    cmd.append(&sctGroupName);
    cmd.append(&backgroundLabel);
    cmd.append(&segmentLengthUm);
    cmd.append(&marginUm);
    cmd.append(&downsampleRatio);
    cmd.append(&qcPositionUm);
    cmd.append(&qcGroup);
    cmd.append(&xdir);
    cmd.append(&debugFileName);
    cmd.append(&computeVisualization);
    cmd.append(&imageFileName);
    cmd.append(&imageDatasetName);

    cmd.description(
        "This program fits local elliptic models to the main axis of the given "
        "segmentation. The dataset may only be moderately bent to make this "
        "work. Specifically, there must be a functional relation between a "
        "straight main axis estimate and the real axis. For the rather "
        "plastic root tip this is usually the case.");

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
        
    //      std::cout << "_downSample = " << _downSample.value() << std::endl;
    std::cout << "infile = '" << inFileName.value() << "'" << std::endl;
    std::cout << "labelDataset = '" << labelDatasetName.value() << "'"
              << std::endl;
    std::cout << "outfile = '" << ofName << "'" << std::endl;
    std::cout << "sctGroup = '" << sctGroupName.value() << "'" << std::endl;
    std::cout << "backgroundLabel = " << backgroundLabel.value() << std::endl;
    std::cout << "segmentLengthUm = " << segmentLengthUm.value() << std::endl;
    std::cout << "margin = " << marginUm.value() << "um" << std::endl;
    std::cout << "downsampleRatio = " << downsampleRatio.value() << std::endl;

    blitz::TinyVector<double, 3> qcPos, axisMainDirection;
    qcPos = -std::numeric_limits<double>::infinity();
    axisMainDirection = -std::numeric_limits<double>::infinity();
    if (qcPositionUm.given())
    {
      qcPos(0) = qcPositionUm[0];
      qcPos(1) = qcPositionUm[1];
      qcPos(2) = qcPositionUm[2];
    }
    else
    {
      if (qcGroup.given())
      {
        try
        {
          BlitzH5File inFile(inFileName.value());
          blitz::Array<blitz::TinyVector<double,3>,1> markerPositionsUm;
          inFile.readDataset(
              markerPositionsUm, qcGroup.value() + "/position_um");
          qcPos = markerPositionsUm(0);
        }
        catch (BlitzH5Error &)
        {}
      }
      else throw CmdLineUsageError()
               << "Missing qc position. Please provide the qc position "
               << "directly using the parameter '--qcPositionUm' or the "
               << "give the annotation group containing the qc annotation "
               << "using the parameter '-q' (--qcGroup)'";
    }
    std::cout << "qcPos = " << qcPos << std::endl;

    if (xdir.given())
    {
      axisMainDirection(0) = xdir[0];
      axisMainDirection(1) = xdir[1];
      axisMainDirection(2) = xdir[2];
      std::cout << "axisMainDirection = " << axisMainDirection << std::endl;
    }

    if (computeVisualization.given())
    {
      if (debugFileName.value() == "")
          throw CmdLineUsageError()
              << "Axis visualization requested, but mandatory parameter "
              << "'--debugfile' missing";
      if (imageFileName.value() == "")
          throw CmdLineUsageError()
              << "Axis visualization requested, but mandatory parameter "
              << "'--imagefile' missing";
      if (imageDatasetName.value() == "")
          throw CmdLineUsageError()
              << "Axis visualization requested, but mandatory parameter "
              << "'--imageDataset' missing";
      std::cout << "imagefile = '" << imageFileName.value() << "'" << std::endl;
      std::cout << "imageDataset = '" << imageDatasetName.value() << "'"
                << std::endl;
    }
    
    iRoCS::ProgressReporterStream pr(std::cout, 0, 0, 100, "\r   ");
    pr.setTaskProgressMin(0);
    pr.setTaskProgressMax(100);

    atb::Array<int,3> segmentation;
    
    pr.updateProgressMessage(
        "Loading segmentation from '" + inFileName.value() + ":" +
        labelDatasetName.value() + "'");
    try
    {
      segmentation.load(inFileName.value(), labelDatasetName.value(), &pr);
    }
    catch (BlitzH5Error &e)
    {
      std::cerr << "Could not load segmentation: " << e.what() << std::endl;
      exit(-1);
    }
    std::cout << "  segmentation.shape() = " << segmentation.shape()
              << std::endl;
    
    std::string debugFile =
        (debugFileName.given()) ? debugFileName.value() : "";

    atb::Array<double,3> *l = NULL, *r = NULL, *p = NULL;
    if (computeVisualization.given())
    {
      l = new atb::Array<double,3>();
      r = new atb::Array<double,3>();
      p = new atb::Array<double,3>();      
    }

    ShellCoordinateTransform sct;
    attachIRoCS(
        sct, segmentation, backgroundLabel.value(),
        downsampleRatio.value(), segmentLengthUm.value(), marginUm.value(),
        qcPos, axisMainDirection, debugFile, l, r, p, &pr);
    delete l;
    delete p;
    if (pr.isAborted())
    {
      delete r;
      exit(-1);
    }
    
    pr.updateProgressMessage(
        "Saving Shell coordinate Transform to '" + ofName + ":" +
        sctGroupName.value() + "'");
    try
    {
      sct.save(ofName, sctGroupName.value());
    }
    catch (BlitzH5Error &e)
    {
      std::cerr << "Could not save '" << ofName << ":" << sctGroupName.value()
                << "': " << e.what() << std::endl;
      delete r;
      exit(-1);
    }

    if (computeVisualization.given())
    {
      pr.updateProgressMessage("Generating visualization of fitting result");
      
      pr.updateProgressMessage(
          "Loading '" + imageFileName.value() + ":" + imageDatasetName.value() +
          "'");
      atb::Array<double,3> data;
      data.load(imageFileName.value(), imageDatasetName.value(), &pr);
      if (blitz::any(data.elementSizeUm() != r->elementSizeUm()))
      {
        pr.updateProgressMessage(
            "Rescaling image data to processing resolution");
        data.rescale(r->elementSizeUm());
      }
      if (blitz::any(data.shape() != r->shape()))
      {
        pr.updateProgressMessage("Cropping image data to processing shape");
        data.resizeAndPreserve(r->shape());
      }
      pr.updateProgressMessage("Normalizing image data to [0, 255] range");
      atb::normalize(data, data, atb::MINMAX);
      pScale(data, 255);
      
      atb::Array<blitz::TinyVector<unsigned char,3>,3> paper(
          (*r).shape(), r->elementSizeUm());
      
      pr.updateProgressMessage("Generating visualization");
#ifdef _OPENMP
#pragma omp parallel for
#endif
      for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(paper.size()); ++i)
      {
        unsigned char val = static_cast<unsigned char>(data.data()[i]);
        paper.data()[i](0) = val;
        paper.data()[i](1) = (std::abs(r->data()[i] - 1) < 0.05) ? 255 : val;
        paper.data()[i](2) = (std::abs(r->data()[i]) < 0.05) ? 255 : val;
      }
      
      pr.updateProgressMessage(
          "Saving visualization to '" + debugFileName.value() + ":/show'");
      try
      {
        paper.save(debugFileName.value(), "/show", 1, &pr);
      }
      catch (BlitzH5Error &e)
      {
        std::cerr << "Could not save visualization: " << e.what() << std::endl;
        exit(-1);
      }
    }
  }
  catch (CmdLineUsageError e)
  {
    cmd.usage();
    exit(0);
  }
  catch (CmdLineUsageHTMLError e)
  {
    cmd.usageHTML(std::cout);
    exit(0);
  }
  catch (CmdLineUsageXMLError e)
  {
    cmd.usageXML(std::cout);
    exit(0);
  }
  catch (CmdLineSyntaxError e)
  {
    cmd.error() << e.str() << std::endl;
    cmd.usage(std::cerr);
    exit(-1);
  }
  return 0;
}
