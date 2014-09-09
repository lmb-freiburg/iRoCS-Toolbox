#include "TrainfileParameters.hh"

#include <libBlitzHdf5/BlitzHdf5Light.hh>

TrainfileParameters::TrainfileParameters(bool needsIRoCSChannel)
        : _needsIRoCSChannel(needsIRoCSChannel), _trainFileName(),
          _dataChannelName(), _annotationChannelName(),
          _iRoCSChannelName(), _cacheFileName()
{}

TrainfileParameters::~TrainfileParameters()
{}
  
void TrainfileParameters::setNeedsIRoCSChannel(bool needsIRoCSChannel)
{
  _needsIRoCSChannel = needsIRoCSChannel;
}

bool TrainfileParameters::needsIRoCSChannel() const
{
  return _needsIRoCSChannel;
}

void TrainfileParameters::setTrainFileName(
    std::string const &name)
{
  _trainFileName = name;
}

std::string TrainfileParameters::trainFileName() const
{
  return _trainFileName;
}

void TrainfileParameters::setDataChannelName(
    std::string const &name)
{
  _dataChannelName = name;
}

std::string TrainfileParameters::dataChannelName() const
{
  return _dataChannelName;
}

void TrainfileParameters::setAnnotationChannelName(
    std::string const &name)
{
  _annotationChannelName = name;
}

std::string TrainfileParameters::annotationChannelName() const
{
  return _annotationChannelName;
}

void TrainfileParameters::setIRoCSChannelName(
    std::string const &name)
{
  _iRoCSChannelName = name;
}

std::string TrainfileParameters::iRoCSChannelName() const
{
  return _iRoCSChannelName;
}

void TrainfileParameters::setCacheFileName(
    std::string const &name)
{
  _cacheFileName = name;
}

std::string TrainfileParameters::cacheFileName() const
{
  return _cacheFileName;
}

std::string TrainfileParameters::check(
    int &nPositiveSamples, int &nSamples)
{
  try
  {
    // Check whether the training file is an hdf5 file
    BlitzH5File inFile(trainFileName());
  }
  catch (BlitzH5Error &e)
  {
    return "Could not open train file '" + trainFileName() + "': " + e.what();
  }
  
  try
  {
    BlitzH5File inFile(trainFileName());
    
    // Check data channel
    if (!inFile.existsDataset(dataChannelName()) ||
        inFile.getDatasetShape(dataChannelName()).size() != 3)
        return "Dataset '" + trainFileName() + ":" + dataChannelName() +
            "' not found or it has wrong dimensionality.";
    
    // Check annotation channel
    if (!inFile.existsDataset(annotationChannelName() + "/position_um") ||
        inFile.getDatasetShape(
            annotationChannelName() + "/position_um").size() != 2 ||
        inFile.getDatasetShape(
            annotationChannelName() + "/position_um")[1] != 3 ||
        !inFile.existsDataset(annotationChannelName() + "/label") ||
        inFile.getDatasetShape(
            annotationChannelName() + "/label").size() != 1 ||
        inFile.getDatasetShape(
            annotationChannelName() + "/position_um")[0] !=
        inFile.getDatasetShape(
            annotationChannelName() + "/label")[0])
    {
      std::stringstream errorString;
      errorString << "'" << trainFileName() << ":" << annotationChannelName()
                  << "' is no valid annotation channel.\n";
      if (inFile.existsDataset(annotationChannelName() + "/position_um") &&
          inFile.existsDataset(annotationChannelName() + "/label"))
      {
        errorString
            << "nPositions = " << inFile.getDatasetShape(
                annotationChannelName() + "/position_um")[0] << "\n"
            << "nLabels    = " << inFile.getDatasetShape(
                annotationChannelName() + "/label")[0] << "\n"
            << "marker dimension = " << inFile.getDatasetShape(
                annotationChannelName() + "/position_um")[1] << "\n"
            << "label dimension  = " << inFile.getDatasetShape(
                annotationChannelName() + "/label").size();
      }
      return errorString.str();
    }
    
    // Check labels
    blitz::Array<int,1> labels;
    inFile.readDataset(labels, annotationChannelName() + "/label");
    for (size_t i = 0; i < labels.size(); ++i)
        if (labels(i) > 0) nPositiveSamples++;
    nSamples += static_cast<int>(labels.size());

    if (_needsIRoCSChannel)
    {
      // Check iRoCS channel
      if (!inFile.existsDataset(iRoCSChannelName() + "/axis/controlPoints") ||
          !inFile.existsDataset(iRoCSChannelName() + "/axis/knots") ||
          !inFile.existsDataset(
              iRoCSChannelName() + "/thickness/controlPoints") ||
          !inFile.existsDataset(iRoCSChannelName() + "/thickness/knots") ||
          inFile.getDatasetShape(
              iRoCSChannelName() + "/axis/controlPoints").size() != 2 ||
          inFile.getDatasetShape(
              iRoCSChannelName() + "/axis/knots").size() != 1 ||
          inFile.getDatasetShape(
              iRoCSChannelName() + "/thickness/controlPoints").size() != 1 ||
          inFile.getDatasetShape(
              iRoCSChannelName() + "/thickness/knots").size() != 1 ||
          inFile.getDatasetShape(
              iRoCSChannelName() + "/axis/controlPoints")[1] != 3)
      {
        std::stringstream errorString;
        errorString << "'" << trainFileName() << ":" << iRoCSChannelName()
                    << "' is no valid iRoCS channel.\n";
        return errorString.str();
      }
    }
  }
  catch (BlitzH5Error &e)
  {
    return "Error while checking '" + trainFileName() + "': " + e.what();
  }
  return "";
}


