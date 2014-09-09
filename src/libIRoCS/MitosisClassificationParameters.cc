#include "MitosisClassificationParameters.hh"

#include <libsvmtl/StDataHdf5.hh>

MitosisClassificationParameters::MitosisClassificationParameters()
        : p_dataChannel(NULL), p_annotationChannel(NULL),
          _cacheFileName("cache.h5"),
          _modelFileName("mitosisClassification_svmModel.h5")
{}

MitosisClassificationParameters::~MitosisClassificationParameters()
{}

DataChannelSpecs* MitosisClassificationParameters::dataChannel() const
{
  return p_dataChannel;
}

void MitosisClassificationParameters::setDataChannel(DataChannelSpecs *channel)
{
  p_dataChannel = channel;
}

AnnotationChannelSpecs*
MitosisClassificationParameters::annotationChannel() const
{
  return p_annotationChannel;
}

void MitosisClassificationParameters::setAnnotationChannel(
    AnnotationChannelSpecs *channel)
{
  p_annotationChannel = channel;
}

std::string MitosisClassificationParameters::cacheFileName() const
{
  return _cacheFileName;
}

void MitosisClassificationParameters::setCacheFileName(
    std::string const &cacheFileName)
{
  _cacheFileName = cacheFileName;
}

std::string MitosisClassificationParameters::modelFileName() const
{
  return _modelFileName;
}

void MitosisClassificationParameters::setModelFileName(
    std::string const &modelFileName)
{
  _modelFileName = modelFileName;
}

std::string MitosisClassificationParameters::check()
{
  if (dataChannel() == NULL) return "The data channel has not been set.";
  if (annotationChannel() == NULL)
      return "The annotation channel has not been set.";

  if (annotationChannel()->markers().size() == 0)
      return "The annotation channel doesn't contain markers to be classified.";
  
  if (annotationChannel()->markerType() != Marker::Nucleus)
      return "The annotation channel must contain Nucleus markers.";
  
  try
  {    
    // Check whether cache file can be created or opened for writing
    if (cacheFileName() != "")
    {
      try
      {
        BlitzH5File cacheFile(cacheFileName(), BlitzH5File::WriteOrNew);
      }
      catch (BlitzH5Error&)
      {
        return "The cache file '" + cacheFileName() +
            "' cannot be created or opened for writing.";
      }
    }
    
    // Check Model file
    try
    {
      svt::StDataHdf5 modelMap(modelFileName().c_str());
      modelMap.setExceptionFlag(true);

      // Check whether model file is an SVM model at all
      try
      {
        std::string kernelType;
        modelMap.getValue("kernel_type", kernelType);
      }
      catch (...)
      {
        return "The model file '" + modelFileName() +
            "' is no valid SVM model.";
      }

      // Check whether it is a multi-class model
      try
      {
        int nClasses;
        modelMap.getValue("n_classes", nClasses);
        return "The model file '" + modelFileName() +
            "' is no valid 2-class SVM model.";
      }
      catch (...)
      {}
      
      // Check whether it contains feature group information
      try
      {
        modelMap.getArraySize("featureGroups");
      }
      catch (...)
      {
        return "The model file '" + modelFileName() +
            "' doesn't contain feature group and normalization "
            "information. Please choose a model created with labelling "
            "or a compatible script that attaches these structures to "
            "the model file.";
      }
    }
    catch (...)
    {
      return "The model file '" + modelFileName() +
          "' doesn't exist or is no valid HDF5 file.";
    }
  }
  catch (std::exception& e)
  {
    return std::string("The supplied parameters contained an error: ") +
        e.what();
  }

  return "";
}

