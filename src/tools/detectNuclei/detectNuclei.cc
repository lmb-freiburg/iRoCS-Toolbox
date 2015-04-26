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

#include <libIRoCS/DetectNucleiWorker.hh>

#include <libProgressReporter/ProgressReporterStream.hh>

class CmdLineVersionError: public CmdLineError {};
class CmdLineLicenseError: public CmdLineError {};

int main(int argc, char **argv)
{
  CmdArgThrow<CmdLineVersionError> versionArg(
      0, "version", "Display version information.");
  CmdArgThrow<CmdLineLicenseError> licenseArg(
      0, "license", "Display licensing information.");

  CmdArgType<std::string> inFileName(
      "<hdf5 file>",
      "The name of the hdf5 file containing the dataset to detect nuclei in",
      CmdArg::isREQ);
  CmdArgType<std::string> datasetName(
      'd', "dataset", "<hdf5 dataset>", "The dataset containing nuclei to "
      "detect", CmdArg::isREQ);
  CmdArgType<std::string> annotationName(
      'n', "nuclei", "<hdf5 group>", "The group to store the detected "
      "nuclei to");
  annotationName.setDefaultValue("/annotation/detector");
  CmdArgType<std::string> cacheFileName(
      'c', "cache", "<hdf5 file>", "The given file is used as feature cache. "
      "If required features are found within the file, they are read instead "
      "of being computed. After computation the cache will be extended by the "
      "new features for later usage.");
  CmdArgType<std::string> modelFileName(
      'm', "model", "<hdf5 svmtl model file>", "The SVM model used for "
      "voxelwise nucleus center probability estimation. The model has to be "
      "generated using svmtl with StDataHDF5 as model map and must contain "
      "additional feature group and normalization information as generated "
      "using the 'Train Detector' plugin in 'labelling'", CmdArg::isREQ);
  CmdArgType<std::string> outFileName(
      'o', "outfile", "<hdf5 file>", "If given, the annotation will be written "
      "to a file of the given name. If omitted the annotations will be written "
      "to the input file");
  CmdArgType<std::string> memoryLimit(
      0, "memoryLimit", "[0-9]+[kKmMgG]*", "If given, the RAM used to store "
      "the feature vectors is restricted to the given amount. This leads to "
      "chunked classification and feature recomputation for each chunk.");

  CmdLine cmd(argv[0], "Nucleus detector");
  cmd.description("Detect cell nuclei in an hdf5 dataset of an Arabidopsis "
                  "root tip");
  
  try
  {
    cmd.append(&versionArg);
    cmd.append(&licenseArg);

    cmd.append(&inFileName);
    cmd.append(&datasetName);
    cmd.append(&annotationName);
    cmd.append(&cacheFileName);
    cmd.append(&modelFileName);
    cmd.append(&outFileName);
    cmd.append(&memoryLimit);
    
    ArgvIter argvIter(--argc, ++argv);
    cmd.parse(argvIter);
    
    size_t mem = 0;
    if (memoryLimit.given())
    {
      if (memoryLimit.value().find_last_of("0123456789") ==
          memoryLimit.value().size() - 1)
          mem = atoi(memoryLimit.value().c_str());
      else
      {
        mem = atoi(memoryLimit.value().substr(
                       0, memoryLimit.value().size() - 1).c_str());
        char unit = memoryLimit.value()[memoryLimit.value().size() - 1];
        switch(unit){
        case 'k' :
        case 'K' :
          mem *= 1024;
          break;
        case 'm' :
        case 'M' :
          mem *= 1024 * 1024;
          break;
        case 'g' :
        case 'G' :
          mem *= 1024 * 1024 * 1024;
          break;
        default :
          break;
        }
      }
    }

    iRoCS::ProgressReporterStream pr(std::cout, 0, 0, 100, "\r ");

    /*---------------------------------------------------------------------
     *  Load data channel
     *---------------------------------------------------------------------*/
    atb::Array<double,3> data;
    try
    {
      pr.updateProgressMessage(
          "Loading '" + inFileName.value() + ":" + datasetName.value() + "'");
      data.load(inFileName.value(), datasetName.value());
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
    std::vector<atb::Nucleus> nuclei;
        
    /*---------------------------------------------------------------------
     *  Run detector
     *---------------------------------------------------------------------*/
    iRoCS::detectNuclei(
        data, nuclei, modelFileName.value(), mem, cacheFileName.value(), &pr);
    if (pr.isAborted()) return -1;
    
    /*---------------------------------------------------------------------
     *  Save Annotation channel
     *---------------------------------------------------------------------*/
    if (outFileName.given())
    {
      pr.updateProgressMessage(
          "Saving '" + outFileName.value() + ":" +
          annotationName.value() + "'");
      atb::Nucleus::saveList(
          nuclei, outFileName.value(), annotationName.value());
    }
    else
    {
      pr.updateProgressMessage(
          "Saving '" + inFileName.value() + ":" + annotationName.value() + "'");
      atb::Nucleus::saveList(
          nuclei, inFileName.value(), annotationName.value());
    }
  }
  catch (CmdLineUsageError &e)
  {
    cmd.usage();
    exit(-1);
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
              << "Copyright (C) 2012-2015 Thorsten Falk ("
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
