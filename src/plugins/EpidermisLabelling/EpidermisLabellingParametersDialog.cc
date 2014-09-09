#include "EpidermisLabellingParametersDialog.hh"

#include <QtGui/QDialogButtonBox>
#include <QtGui/QMessageBox>
#include <QtCore/QSettings>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPushButton>

#include <liblabelling_qt4/MultiChannelModel.hh>

#include <libsvmtl/StDataHdf5.hh>

#include <fstream>

EpidermisLabellingParametersDialog::EpidermisLabellingParametersDialog(
    LabellingMainWidget* mainWidget, QWidget* parent, Qt::WindowFlags f)
        : QDialog(parent, f), p_mainWidget(mainWidget)
{
  QVBoxLayout* mainLayout = new QVBoxLayout;

  QFrame* parameterPanel = new QFrame;
  parameterPanel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  QVBoxLayout* parameterLayout = new QVBoxLayout;
  
  p_dataChannelSelector = new ChannelSelectionControlElement(
      tr("Data channel:"), mainWidget->multiChannelModel(),
      ChannelSpecs::Data | ChannelSpecs::Visualization);
  parameterLayout->addWidget(p_dataChannelSelector);

  p_annotationChannelSelector = new ChannelSelectionControlElement(
      tr("Annotation channel:"), p_mainWidget->multiChannelModel(),
      ChannelSpecs::Annotation);
  parameterLayout->addWidget(p_annotationChannelSelector);

  p_modelFileNameControlElement = new FileNameSelectionControlElement(
      tr("Model File:"), "", true, "HDF5 (*.h5)");
  parameterLayout->addWidget(p_modelFileNameControlElement);
  
  p_cacheFileControlElement = new FileNameSelectionControlElement(
      tr("Cache File:"), "", false, "HDF5 (*.h5)");
  parameterLayout->addWidget(p_cacheFileControlElement);  
  
  p_forceFeatureComputationControl = new BoolControlElement(
      tr("Force feature computation:"), false);
  parameterLayout->addWidget(p_forceFeatureComputationControl);

  parameterPanel->setLayout(parameterLayout);
  mainLayout->addWidget(parameterPanel);
  
  QDialogButtonBox* buttonBox = new QDialogButtonBox(
      QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);
  connect(buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()),
          SLOT(checkAndAccept()));
  buttonBox->button(QDialogButtonBox::Cancel)->setAutoDefault(false);
  connect(buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()),
          SLOT(reject()));
  mainLayout->addWidget(buttonBox);

  // Load field values from settings
  QSettings settings;

  QString channelName;
  QString annotationName;
  QString modelName;
  QString cacheName;
  if (p_mainWidget->fileName() == "")
  {
    channelName = settings.value(
        "iRoCSPipeline/DataChannel",
        p_dataChannelSelector->toStdString().c_str()).toString();
    annotationName = settings.value(
        "iRoCSPipeline/AnnotationChannel",
        p_annotationChannelSelector->toStdString().c_str()).toString();
    modelName = settings.value(
        "iRoCSPipeline/EpidermisLabellingModel", "").toString();
    cacheName = "";
  }
  else
  {
    channelName = settings.value(
        ("iRoCSPipeline/DataChannel/" + p_mainWidget->fileName()).c_str(),
        p_dataChannelSelector->toStdString().c_str()).toString();
    annotationName = settings.value(
        ("iRoCSPipeline/AnnotationChannel/" + p_mainWidget->fileName()).c_str(),
        p_annotationChannelSelector->toStdString().c_str()).toString();
    modelName = settings.value(
        ("iRoCSPipeline/EpidermisLabellingModel/" +
         p_mainWidget->fileName()).c_str(),
        settings.value(
            "iRoCSPipeline/EpidermisLabellingModel", "").toString()).toString();
    cacheName = settings.value(
        ("iRoCSPipeline/CacheFile/" + p_mainWidget->fileName()).c_str(),
        "").toString();
  }
  if (p_dataChannelSelector->contains(channelName.toStdString()))
      p_dataChannelSelector->setValue(channelName.toStdString());
  if (p_annotationChannelSelector->contains(annotationName.toStdString()))
      p_annotationChannelSelector->setValue(annotationName.toStdString());
  p_modelFileNameControlElement->setValue(modelName.toStdString());
  p_cacheFileControlElement->setValue(cacheName.toStdString());
  p_forceFeatureComputationControl->setValue(false);

  setLayout(mainLayout);
}

EpidermisLabellingParametersDialog::~EpidermisLabellingParametersDialog()
{}

ChannelSpecs* EpidermisLabellingParametersDialog::dataChannel() const
{
  return p_mainWidget->multiChannelModel()->channel(
      p_dataChannelSelector->toStdString());
}

void EpidermisLabellingParametersDialog::setDataChannel(
    ChannelSpecs *channel)
{
  if (p_dataChannelSelector->contains(channel->name()))
      p_dataChannelSelector->setValue(channel->name());
}

AnnotationChannelSpecs *EpidermisLabellingParametersDialog::annotationChannel()
    const
{
  return static_cast<AnnotationChannelSpecs*>(
      p_mainWidget->multiChannelModel()->channel(
          p_annotationChannelSelector->toStdString()));
}

void EpidermisLabellingParametersDialog::setAnnotationChannel(
    AnnotationChannelSpecs *channel)
{
  if (p_annotationChannelSelector->contains(channel->name()))
  {
    p_annotationChannelSelector->setValue(channel->name());
  }
}

std::string EpidermisLabellingParametersDialog::cacheFileName() const
{
  return p_cacheFileControlElement->value();
}

void EpidermisLabellingParametersDialog::setCacheFileName(
    std::string const &cacheFileName)
{
  p_cacheFileControlElement->setValue(cacheFileName);
}

std::string EpidermisLabellingParametersDialog::modelFileName() const
{
  return p_modelFileNameControlElement->value();
}

void EpidermisLabellingParametersDialog::setModelFileName(
    std::string const &modelFileName)
{
  p_modelFileNameControlElement->setValue(modelFileName);
}

bool EpidermisLabellingParametersDialog::forceFeatureComputation() const
{
  return p_forceFeatureComputationControl->value();
}

void EpidermisLabellingParametersDialog::setForceFeatureComputation(bool force)
{
  p_forceFeatureComputationControl->setValue(force);
}

std::string EpidermisLabellingParametersDialog::check()
{
  if (dataChannel() == NULL) return "The data channel has not been set.";
  if (annotationChannel() == NULL)
      return "The annotation channel has not been set.";

  if (annotationChannel()->markers().size() == 0)
      return "The annotation channel doesn't contain markers to be classified.";
  
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
      }
      catch (...)
      {
        return "The model file '" + modelFileName() +
            "' is no valid multi-class SVM model.";
      }
      
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

void EpidermisLabellingParametersDialog::checkAndAccept()
{
  std::string res = check();
  
  if (res != "")
  {
    p_mainWidget->showError(res.c_str());
    return;
  }
  
  QSettings settings;

  if (p_mainWidget->fileName() == "")
  {
    settings.setValue(
        "iRoCSPipeline/DataChannel",
        p_dataChannelSelector->toStdString().c_str());
    settings.setValue(
        "iRoCSPipeline/AnnotationChannel",
        p_annotationChannelSelector->toStdString().c_str());
    settings.setValue(
        "iRoCSPipeline/EpidermisLabellingModel",
        p_modelFileNameControlElement->toStdString().c_str());
  }
  else
  {
    settings.setValue(
        ("iRoCSPipeline/DataChannel/" + p_mainWidget->fileName()).c_str(),
        p_dataChannelSelector->toStdString().c_str());
    settings.setValue(
        ("iRoCSPipeline/AnnotationChannel/" + p_mainWidget->fileName()).c_str(),
        p_annotationChannelSelector->toStdString().c_str());
    settings.setValue(
        ("iRoCSPipeline/EpidermisLabellingModel/" +
         p_mainWidget->fileName()).c_str(),
        p_modelFileNameControlElement->toStdString().c_str());
    settings.setValue(
        ("iRoCSPipeline/CacheFile/" + p_mainWidget->fileName()).c_str(),
        p_cacheFileControlElement->toStdString().c_str());
  }
  
  accept();
}
