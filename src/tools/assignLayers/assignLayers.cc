#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#include <libcmdline/CmdLine.hh>
#include <libcmdline/ArgvIter.hh>

#include <libIRoCS/LayerAssignmentWorker.hh>

#include <libArrayToolbox/iRoCS.hh>

#include <libProgressReporter/ProgressReporterStream.hh>

int main(int argc, char **argv)
{
  CmdArgType<std::string> inFileName(
      "<hdf5 file>",
      "The name of the hdf5 file containing the dataset to classify nuclei in",
      CmdArg::isREQ);
  CmdArgType<std::string> datasetName(
      'd', "dataset", "<hdf5 dataset>", "The raw dataset", CmdArg::isREQ);
  CmdArgType<std::string> annotationName(
      'n', "nuclei", "<nucleus group>", "The group to load the nuclei to "
      "classify from");
  annotationName.setDefaultValue("/annotation/detector");
  CmdArgType<std::string> outAnnotationName(
      0, "outgroup", "<nucleus group>",
      "The group to store the classified nuclei to "
      "(Default = <nucleus group>)");
  CmdArgType<std::string> iRoCSName(
      'a', "axis", "<hdf5 group>", "The group to load the root axis from");
  iRoCSName.setDefaultValue("/annotation/axis");
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
      "back to the old location");
  CmdArgSwitch cacheCoordinates(
      0, "cacheCoordinates", "If given, the voxelwise coordinates will be "
      "computed and written to the cache file. This is very slow, and only "
      "intended for debugging");
  CmdArgSwitch updateMitoses(
      0, "updateMitoses", "If given, the nuclei are not onyl classified by "
      "layer, but also by mitotic state. Usually this is not necessary, "
      "because the mitotic state is already set during Epidermis labelling.");
  CmdArgSwitch forceFeatureComputation(
      'f', "forceFeatureComputation", "If this flag is given the features "
      "will be recomputed even if the needsFeatureComputation flag is not "
      "set.");

  CmdLine cmd(argv[0], "Arabidopsis layer Assignment");
  cmd.description("Classify the nuclei of an Arabidopsis root tip by layer");
  
  try
  {
    cmd.append(&inFileName);
    cmd.append(&datasetName);
    cmd.append(&annotationName);
    cmd.append(&outAnnotationName);
    cmd.append(&iRoCSName);
    cmd.append(&cacheFileName);
    cmd.append(&modelFileName);
    cmd.append(&outFileName);
    cmd.append(&cacheCoordinates);
    cmd.append(&updateMitoses);
    cmd.append(&forceFeatureComputation);
    
    ArgvIter argvIter(--argc, ++argv);
    cmd.parse(argvIter);

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
     *  Load annotation channel
     *---------------------------------------------------------------------*/ 
    pr.updateProgressMessage(
        "Loading '" + inFileName.value() + ":" + annotationName.value() + "'");
    std::vector<atb::Nucleus> nuclei;
    atb::Nucleus::loadList(nuclei, inFileName.value(), annotationName.value());
        
    /*---------------------------------------------------------------------
     *  Load iRoCS channel
     *---------------------------------------------------------------------*/ 
    pr.updateProgressMessage(
        "Loading '" + inFileName.value() + ":" + iRoCSName.value() + "'");
    atb::IRoCS iRoCS;
    iRoCS.load(inFileName.value(), iRoCSName.value());

    /*---------------------------------------------------------------------
     *  Run classifier
     *---------------------------------------------------------------------*/
    iRoCS::assignLayers(
        data, nuclei, iRoCS, modelFileName.value(), cacheFileName.value(),
        updateMitoses.given(), cacheCoordinates.given(),
        forceFeatureComputation.given(), &pr);
    if (pr.isAborted()) return -1;
    
    /*---------------------------------------------------------------------
     *  Save Annotation channel
     *---------------------------------------------------------------------*/
    std::string outFile = (outFileName.given()) ?
        outFileName.value() : inFileName.value();
    std::string outGroup = (outAnnotationName.given()) ?
        outAnnotationName.value() : annotationName.value();
    pr.updateProgressMessage("Saving '" + outFile + ":" + outGroup + "'");
    atb::Nucleus::saveList(nuclei, outFile, outGroup);
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
