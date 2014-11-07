#include "TrainEpidermisLabellingTrainfileParametersItem.hh"
#include "TrainEpidermisLabellingParametersDialog.hh"

#include <QtCore/QSettings>

TrainEpidermisLabellingTrainfileParametersItem::
TrainEpidermisLabellingTrainfileParametersItem(
    LabellingMainWidget *mainWidget, std::string const &filename)
        : QTreeWidgetItem(), TrainfileParameters(false),
          p_mainWidget(mainWidget)
{
  p_trainFileSelector = new FileNameSelectionControlElement(
      "", filename, true, "hdf5 (*.h5)", p_mainWidget);
  
  QString dataChannelName = "";
  QString annotationChannelName = "";
  QString cacheFileName = "";
  QSettings settings;
  if (filename == "")
  {
    dataChannelName =
        settings.value("iRoCSPipeline/DataChannel", "").toString();
    annotationChannelName =
        settings.value("iRoCSPipeline/AnnotationChannel", "").toString();
  }
  else
  {
    dataChannelName =
        settings.value(
            ("iRoCSPipeline/DataChannel/" + filename).c_str(),
            settings.value(
                "iRoCSPipeline/DataChannel", "").toString()).toString();
    annotationChannelName =
        settings.value(
            ("iRoCSPipeline/AnnotationChannel/" + filename).c_str(),
            settings.value(
                "iRoCSPipeline/AnnotationChannel", "").toString()).toString();
    cacheFileName =
        settings.value(
            ("iRoCSPipeline/CacheFile/" + filename).c_str(), "").toString();
  }
  p_dataChannelControlElement = new HDF5SelectionControlElement(
      "", filename.c_str(), dataChannelName.toStdString(), NULL,
      ChannelSpecs::Data | ChannelSpecs::Visualization, p_mainWidget);
  QObject::connect(
      p_trainFileSelector, SIGNAL(valueChanged(QString const &)),
      p_dataChannelControlElement, SLOT(setFileName(QString const &)));
  
  p_annotationChannelControlElement = new HDF5SelectionControlElement(
      "", filename.c_str(), annotationChannelName.toStdString(), NULL,
      ChannelSpecs::Annotation, p_mainWidget);
  QObject::connect(
      p_trainFileSelector, SIGNAL(valueChanged(QString const &)),
      p_annotationChannelControlElement,
      SLOT(setFileName(QString const &)));
  
  p_cacheFileSelector = new FileNameSelectionControlElement(
      "", cacheFileName.toStdString(), false, "hdf5 (*.h5)", p_mainWidget);
}

TrainEpidermisLabellingTrainfileParametersItem::
~TrainEpidermisLabellingTrainfileParametersItem()
{}
  
void TrainEpidermisLabellingTrainfileParametersItem::setTrainFileName(
    std::string const &name)
{
  p_trainFileSelector->setValue(name);
}

std::string TrainEpidermisLabellingTrainfileParametersItem::trainFileName() const
{
  return p_trainFileSelector->value();
}

void TrainEpidermisLabellingTrainfileParametersItem::setDataChannelName(
    std::string const &name)
{
  p_dataChannelControlElement->setValue(name);
}

std::string
TrainEpidermisLabellingTrainfileParametersItem::dataChannelName() const
{
  return p_dataChannelControlElement->value();
}

void TrainEpidermisLabellingTrainfileParametersItem::setAnnotationChannelName(
    std::string const &name)
{
  p_annotationChannelControlElement->setValue(name);
}

std::string
TrainEpidermisLabellingTrainfileParametersItem::annotationChannelName() const
{
  return p_annotationChannelControlElement->value();
}

void TrainEpidermisLabellingTrainfileParametersItem::setCacheFileName(
    std::string const &name)
{
  p_cacheFileSelector->setValue(name);
}

std::string
TrainEpidermisLabellingTrainfileParametersItem::cacheFileName() const
{
  return p_cacheFileSelector->value();
}

std::string TrainEpidermisLabellingTrainfileParametersItem::check(
    int &nPositiveSamples, int &nSamples)
{
  // Reset cell background to white
  setBackground(0, QBrush(QColor(255, 255, 255)));
  setBackground(1, QBrush(QColor(255, 255, 255)));
  setBackground(2, QBrush(QColor(255, 255, 255)));
  setBackground(3, QBrush(QColor(255, 255, 255)));

  try
  {
    // Check whether the training file is an hdf5 file
    BlitzH5File inFile(trainFileName());
    setBackground(0, QBrush(QColor(128, 255, 128)));

    // Check data channel
    if (inFile.existsDataset(dataChannelName()) &&
        inFile.getDatasetShape(dataChannelName()).size() == 3)
    {
      setBackground(1, QBrush(QColor(128, 255, 128)));
    }
    else
    {
      setBackground(1, QBrush(QColor(255, 128, 128)));
      return "Dataset '" + trainFileName() + ":" + dataChannelName() +
          "' not found";
    }

    // Check annotation channel
    if (inFile.existsDataset(annotationChannelName() + "/position_um") &&
        inFile.getDatasetShape(
            annotationChannelName() + "/position_um").size() == 2 &&
        inFile.getDatasetShape(
            annotationChannelName() + "/position_um")[1] == 3 &&
        inFile.existsDataset(annotationChannelName() + "/label") &&
        inFile.getDatasetShape(
            annotationChannelName() + "/label").size() == 1 &&
        inFile.getDatasetShape(
            annotationChannelName() + "/position_um")[0] ==
        inFile.getDatasetShape(
            annotationChannelName() + "/label")[0])
    {
      // Check labels
      blitz::Array<int,1> labels;
      inFile.readDataset(labels, annotationChannelName() + "/label");
      for (size_t i = 0; i < labels.size(); ++i)
          if (labels(i) > 0) nPositiveSamples++;
      nSamples += static_cast<int>(labels.size());

      setBackground(2, QBrush(QColor(128, 255, 128)));
    }
    else
    {
      setBackground(2, QBrush(QColor(255, 128, 128)));
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

    setBackground(3, QBrush(QColor(128, 255, 128)));

    QSettings settings;
    settings.setValue(
        ("iRoCSPipeline/DataChannel/" + trainFileName()).c_str(),
        dataChannelName().c_str());
    settings.setValue(
        ("iRoCSPipeline/AnnotationChannel/" + trainFileName()).c_str(),
        annotationChannelName().c_str());
    settings.setValue(
        ("iRoCSPipeline/CacheFile/" + trainFileName()).c_str(),
        cacheFileName().c_str());
  }
  catch (BlitzH5Error &e)
  {
    setBackground(0, QBrush(QColor(255, 128, 128)));
    return "Error while checking '" + trainFileName() + "': " + e.what();
  }    

  return "";  
}

