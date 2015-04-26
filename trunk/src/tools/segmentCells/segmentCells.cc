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

#include <libIRoCS/RootSegmentationWorker.hh>

#include <libcmdline/CmdLine.hh>
#include <libcmdline/ArgvIter.hh>

#include <libProgressReporter/ProgressReporterStream.hh>

#include <libBlitzHdf5/BlitzHdf5Light.hh>

class CmdLineVersionError: public CmdLineError {};
class CmdLineLicenseError: public CmdLineError {};

int main(int argc, char *argv[])
{
  /*---------------------------------------------------------------------
   *  Specify command line arguments and descriptions
   *---------------------------------------------------------------------*/
  CmdArgThrow<CmdLineVersionError> versionArg(
      0, "version", "Display version information.");
  CmdArgThrow<CmdLineLicenseError> licenseArg(
      0, "license", "Display licensing information.");

  // IO parameters
  CmdArgType<std::string> inFileName(
      "<infile>", "input file (HDF5) containing the image", CmdArg::isREQ);
  CmdArgType<std::string> datasetName(
      'i', "dataset", "<hdf5 dataset name>", "HDF5 dataset name containing "
      "the volume to segment", CmdArg::isREQ);
  CmdArgType<std::string> outFileName(
      'o', "outfile", "<hdf5 filename>", "The name of the output hdf5 file to "
      "save the segmentation to.");
  outFileName.setDefaultValue("<infile>");
  CmdArgType<std::string> outDatasetName(
      0, "outDataset", "<hdf5 dataset name>", "The name of the dataset to "
      "save the segmentation result to.");
  outDatasetName.setDefaultValue("/segmentation/cellularMasks");
  CmdArgType<std::string> debugFileName(
      0, "debugfile", "<string>", "Write intermediate information to a file "
      "with the given name.");
  CmdArgType<double> processingElementSizeUm(
      'e', "processingElementSizeUm", "<double>", "The cubic processing "
      "element size in micrometers. The dataset is resacled to this voxel "
      "edge length prior to processing. Values <= 0 mean, that the volume "
      "will be rescaled to cubic voxels with an edge length of the shortest "
      "edge length of the original dataset (maximum size increase).");
  processingElementSizeUm.setDefaultValue(0.0);

  // Pre-processing
  CmdArgType<int> normalizationType(
      'n', "normalizationType", "[0-3]", "specify the way to normalize the "
      "data: 0-none, 1-MINMAX, 2-STDDEV, 3-ZERO_MEAN_STDDEV");
  normalizationType.setDefaultValue(2);
  CmdArgType<double> gamma(
      'g', "gamma", "<float>", "specify the gamma correction, set to 0 to "
      "disable it");
  gamma.setDefaultValue(1);
  CmdArgType<int> medianWidthPx(
      'm', "medianWidthPx", "<int>", "Apply median filter with the given "
      "filter width in pixels prior to segmentation.");
  medianWidthPx.setDefaultValue(0);

  //variance normalization
  CmdArgType<double> varSigmaUm(
      's', "varianceNormalizationSigmaUm", "<double>",
      "specify the Gaussian kernel width (standard deviation) in micrometers "
      "for local variance normalization. Values <=0 disable the "
      "normalization.");
  varSigmaUm.setDefaultValue(20.0);
  CmdArgType<double> epsilon(
      0, "epsilon", "<double>", "specify the epsilon value that is used to "
      "avoid infinite values in the local variance normalization: "
      "I = I / (local_dev + epsilon). The default value is generally a good "
      "choice.");
  epsilon.setDefaultValue(1.0);

  //diffusion
  CmdArgSwitch applyDiffusion(
      'd', "applyDiffusion", "Pre-process data using anisotropic diffusion.");
  CmdArgType<double> kappa(
      'k', "kappa", "<float>", "Diffusivity = exp(-g / (kappa * kappa))");
  kappa.setDefaultValue(0.2);
  CmdArgType<float> tau(
      0, "tau", "<float>", "Diffusion time step. Reduce this value if you "
      "experience numerical problems. Normally it is fine at the default.");
  tau.setDefaultValue(0.0625);
  CmdArgType<float> zCompensationFactor(
      0, "zCompensationFactor", "<float>", "specify the compensation factor "
      "applied to eigenvalues with eigenvectors in Z direction. If the image "
      "quality in z-direction is very poor, increasing this value might help "
      "a little.");
  zCompensationFactor.setDefaultValue(1);
  CmdArgType<int> nDiffusionIterations(
      0, "nDiffusionIterations", "<int>", "Number of diffusion iterations");
  nDiffusionIterations.setDefaultValue(10);

  //watershed
  CmdArgType<float> hessianSigmaUm(
      0, "hessianSigmaUm", "<float>", "specify the gaussian sigma for "
      "computing the hessian matrix (and gradient) (in um). For values <=0 "
      "this parameter is automatically chosen to be the processing element "
      "size. Overriding it will most probably reduce the segmentation "
      "quality.");
  hessianSigmaUm.setDefaultValue(0.0);
  CmdArgType<float> edgeThreshold(
      't', "edgeThreshold", "<float>", "Structures with smallest "
      "Hessian eigenvalue below this threshold will be regarded as edge "
      "candidates. Increase it, if edges are missing in the resulting "
      "segmentation. Bright edges on dark ground should always have at least "
      "one negative eigenvalue, so only negative values make sense here.");
  edgeThreshold.setDefaultValue(-0.2);
  CmdArgType<int> boundaryThicknessPx(
      0, "boundaryThicknessPx", "<int>", "The segment boundaries will be "
      "widened by this amount (in pixels)");
  boundaryThicknessPx.setDefaultValue(0);
  CmdArgType<float> minimumVolumeUm3(
      'c', "minimumCellVolumeUm3", "<float>", "specify the minimum volume "
      "acceptable as a cell (in um^3)");
  minimumVolumeUm3.setDefaultValue(60);

  CmdLine cmd(
      argv[0], "Apply a watershed segmentation to extract the cells in the "
      "given image volume. Prior to segmentation several pre-processing "
      "filters can be applied: Gamma-correction, median filtering and "
      "anisotropic diffusion.");

  try
  {
    cmd.append(&versionArg);
    cmd.append(&licenseArg);
  
    cmd.append(&inFileName);
    cmd.append(&datasetName);
    cmd.append(&outFileName);
    cmd.append(&outDatasetName);
    cmd.append(&debugFileName);
    cmd.append(&processingElementSizeUm);

    cmd.append(&normalizationType);
    cmd.append(&gamma);
    cmd.append(&medianWidthPx);

    cmd.append(&varSigmaUm);
    cmd.append(&epsilon);

    cmd.append(&applyDiffusion);
    cmd.append(&kappa);
    cmd.append(&tau);
    cmd.append(&zCompensationFactor);
    cmd.append(&nDiffusionIterations);

    cmd.append(&hessianSigmaUm);
    cmd.append(&edgeThreshold);
    cmd.append(&boundaryThicknessPx);
    cmd.append(&minimumVolumeUm3);

    cmd.description("apply segmentation on data");

    /*---------------------------------------------------------------------
     *  Parse command line
     *---------------------------------------------------------------------*/
    ArgvIter arg_iter(--argc, ++argv);
    cmd.parse(arg_iter);

    if (normalizationType.value() < 0 || normalizationType.value() > 3)
        throw CmdLineUsageError()
            << "'normalizationType' must be one of 0, 1, 2, 3";

    std::string ofName =
        (outFileName.value() == "" || outFileName.value() == "<infile>") ?
        inFileName.value() : outFileName.value();

    /*---------------------------------------------------------------------
     *  print given values
     *---------------------------------------------------------------------*/
    std::cout << "infile = '" << inFileName.value() << "'" << std::endl;
    std::cout << "dataset = '" << datasetName.value() << "'" << std::endl;
    std::cout << "outfile = '" << ofName << "'" << std::endl;
    std::cout << "outDataset = '" << outDatasetName.value() << "'" << std::endl;
    std::cout << "debugFileName = "
              << (debugFileName.given() ? ("'" + debugFileName.value() + "'") :
                  std::string("<none>")) << std::endl;
    std::cout << "processingElementSizeUm = " << processingElementSizeUm.value()
              << std::endl;

    std::cout << "normalizationType = " << normalizationType.value()
              << std::endl;
    std::cout << "gamma = " << gamma.value() << std::endl;
    std::cout << "medianWidthPx = " << medianWidthPx.value() << " px"
              << std::endl;

    std::cout << "varSigmaUm = " << varSigmaUm.value() << " um" << std::endl;
    std::cout << "epsilon = " << epsilon.value() << std::endl;

    std::cout << "applyDiffusion = "
              << (applyDiffusion.given() ? "<yes>" : "<no>") << std::endl;
    std::cout << "kappa = " << kappa.value() << std::endl;
    std::cout << "tau = " << tau.value() << std::endl;
    std::cout << "zCompensationFactor = " << zCompensationFactor.value()
              << std::endl;
    std::cout << "nDiffusionIterations = " << nDiffusionIterations.value()
              << std::endl;

    std::cout << "hessianSigmaUm = " << hessianSigmaUm.value() << " um"
              << std::endl;
    std::cout << "edgeThreshold = " << edgeThreshold.value() << std::endl;
    std::cout << "boundaryThicknessPx = " << boundaryThicknessPx.value()
              << " px" << std::endl;
    std::cout << "minimumVolumeUm3 = " << minimumVolumeUm3.value()
              << " um^3" << std::endl;

    iRoCS::ProgressReporterStream pr(std::cout, 0, 0, 100, "\r   ");
    pr.setTaskProgressMin(0);
    pr.setTaskProgressMax(100);

    std::string debugFile =
        debugFileName.given() ? debugFileName.value() : std::string("");

    pr.updateProgressMessage(
        "Loading '" + inFileName.value() + ":" + datasetName.value() + "'");
    atb::Array<double, 3> data;
    try
    {
      data.load(inFileName.value(), datasetName.value());
    }
    catch (BlitzH5Error &e)
    {
      std::cerr << "Could not load '" << inFileName.value() << ":"
                << datasetName.value() << "': " << e.what() << std::endl;
      exit(-1);
    }

    atb::Array<int,3> segmentation;
    segmentCells(
        data, segmentation, gamma.value(), normalizationType.value(),
        medianWidthPx.value(), processingElementSizeUm.value(),
        varSigmaUm.value(), epsilon.value(), hessianSigmaUm.value(),
        applyDiffusion.given(), nDiffusionIterations.value(),
        zCompensationFactor.value(), kappa.value(), tau.value(),
        edgeThreshold.value(), minimumVolumeUm3.value(),
        boundaryThicknessPx.value(), debugFile, &pr);

    pr.updateProgressMessage(
        "Saving segmentation result to '" + ofName + ":" +
        outDatasetName.value() + "'");
    try
    {
      segmentation.save(ofName, outDatasetName.value(), 1, &pr);
    }
    catch (BlitzH5Error &e)
    {
      std::cerr << "Could not write segmentation result: " << e.what()
                << std::endl;
      exit(-1);
    }
    
    std::string markerGroup = BlitzH5File::simplifyGroupDescriptor(
        outDatasetName.value());
    size_t slashIndex = markerGroup.rfind("/");
    if (slashIndex != std::string::npos)
        markerGroup = markerGroup.substr(0, slashIndex);
    else markerGroup = "/";

    pr.updateProgressMessage(
        "Saving parameters to '" + ofName + ":" + markerGroup + "'");
    try
    {
      BlitzH5File outFile(outFileName.value(), BlitzH5File::WriteOrNew);
      outFile.writeAttribute(
          normalizationType.value(), "normalizationType", markerGroup);
      outFile.writeAttribute(
          processingElementSizeUm.value(), "processingElementSizeUm",
          markerGroup);
      outFile.writeAttribute(kappa.value(), "kappa", markerGroup);
      outFile.writeAttribute(
          (applyDiffusion.given() ? 1 : 0), "applyDiffusion", markerGroup);
      outFile.writeAttribute(tau.value(), "tau", markerGroup);
      outFile.writeAttribute(
          nDiffusionIterations.value(), "nDiffusionIterations", markerGroup);
      outFile.writeAttribute(
          hessianSigmaUm.value(), "hessianSigmaUm", markerGroup);
      outFile.writeAttribute(
          edgeThreshold.value(), "edgeThreshold", markerGroup);
      outFile.writeAttribute(
          zCompensationFactor.value(), "zCompensationFactor", markerGroup);
      outFile.writeAttribute(
          minimumVolumeUm3.value(), "minimumVolumeUm3", markerGroup);
      outFile.writeAttribute(
          boundaryThicknessPx.value(), "boundaryThicknessPx", markerGroup);
    }
    catch (BlitzH5Error& e)
    {
      std::cerr << "Could not save parameters to '" << debugFileName.value()
                << "': " << e.str() << std::endl;
    }
  }
  catch (CmdLineUsageError e)
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
  catch (std::exception &e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
    exit(-1);
  }
  catch (...)
  {
    std::cerr << "Unknown error" << std::endl;
    exit(-1);
  }
  return 0;
}
