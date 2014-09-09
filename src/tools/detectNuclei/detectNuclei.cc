#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#include <libcmdline/CmdLine.hh>
#include <libcmdline/ArgvIter.hh>

#include <libIRoCS/DetectNucleiWorker.hh>

#include <libProgressReporter/ProgressReporterStream.hh>

int main(int argc, char **argv)
{
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
