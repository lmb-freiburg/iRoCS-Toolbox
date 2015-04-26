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

#include "LayerAssignmentParametersDialog.hh"

#include <QtGui/QDialogButtonBox>
#include <QtGui/QMessageBox>
#include <QtCore/QSettings>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPushButton>

#include <liblabelling_qt4/MultiChannelModel.hh>

#include <libsvmtl/StDataHdf5.hh>

LayerAssignmentParametersDialog::LayerAssignmentParametersDialog(
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
      tr("Annotation channel:"), mainWidget->multiChannelModel(),
      ChannelSpecs::Annotation);
  parameterLayout->addWidget(p_annotationChannelSelector);

  p_iRoCSChannelSelector = new ChannelSelectionControlElement(
      tr("Axis Channel:"), mainWidget->multiChannelModel(),
      ChannelSpecs::IRoCS);
  parameterLayout->addWidget(p_iRoCSChannelSelector);
  
  p_modelFileNameControlElement = new FileNameSelectionControlElement(
      tr("Model File:"), "", true, "HDF5 (*.h5)");
  parameterLayout->addWidget(p_modelFileNameControlElement);
  
  p_cacheFileControlElement = new FileNameSelectionControlElement(
      tr("Cache File:"), "", false, "HDF5 (*.h5)");
  parameterLayout->addWidget(p_cacheFileControlElement);  
  
  p_forceFeatureComputationControl = new BoolControlElement(
      tr("Force feature computation:"), false);
  parameterLayout->addWidget(p_forceFeatureComputationControl);

  p_cacheCoordinates = new BoolControlElement(
      tr("Compute voxelwise coordinates:"), false);
  parameterLayout->addWidget(p_cacheCoordinates);

  p_updateMitoses = new BoolControlElement(
      tr("Re-classify mitotic state:"), false);
  parameterLayout->addWidget(p_updateMitoses);

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
  QString iRoCSName;
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
    iRoCSName = settings.value(
        "iRoCSPipeline/AxisChannel",
        p_iRoCSChannelSelector->toStdString().c_str()).toString();
    modelName = settings.value(
        "iRoCSPipeline/LayerAssignmentModel", "").toString();
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
    iRoCSName = settings.value(
        ("iRoCSPipeline/AxisChannel/" + p_mainWidget->fileName()).c_str(),
        p_iRoCSChannelSelector->toStdString().c_str()).toString();
    modelName = settings.value(
        ("iRoCSPipeline/LayerAssignmentModel/" +
         p_mainWidget->fileName()).c_str(),
        settings.value(
            "iRoCSPipeline/LayerAssignmentModel", "").toString()).toString();
    cacheName = settings.value(
        ("iRoCSPipeline/CacheFile/" + p_mainWidget->fileName()).c_str(),
        "").toString();
  }
  if (p_dataChannelSelector->contains(channelName.toStdString()))
      p_dataChannelSelector->setValue(channelName.toStdString());
  if (p_annotationChannelSelector->contains(annotationName.toStdString()))
      p_annotationChannelSelector->setValue(annotationName.toStdString());
  if (p_iRoCSChannelSelector->contains(iRoCSName.toStdString()))
      p_iRoCSChannelSelector->setValue(iRoCSName.toStdString());
  p_modelFileNameControlElement->setValue(modelName.toStdString());
  p_cacheFileControlElement->setValue(cacheName.toStdString());
  
  setLayout(mainLayout);
}

LayerAssignmentParametersDialog::~LayerAssignmentParametersDialog()
{}

ChannelSpecs* LayerAssignmentParametersDialog::dataChannel() const
{
  return p_mainWidget->multiChannelModel()->channel(
      p_dataChannelSelector->toStdString());
}

void LayerAssignmentParametersDialog::setDataChannel(
    ChannelSpecs *channel)
{
  if (p_dataChannelSelector->contains(channel->name()))
      p_dataChannelSelector->setValue(channel->name());
}

AnnotationChannelSpecs *LayerAssignmentParametersDialog::annotationChannel()
    const
{
  return static_cast<AnnotationChannelSpecs*>(
      p_mainWidget->multiChannelModel()->channel(
          p_annotationChannelSelector->toStdString()));
}

void LayerAssignmentParametersDialog::setAnnotationChannel(
    AnnotationChannelSpecs *channel)
{
  if (p_annotationChannelSelector->contains(channel->name()))
      p_annotationChannelSelector->setValue(channel->name());
}

IRoCSChannelSpecs *LayerAssignmentParametersDialog::iRoCSChannel() const
{
  return static_cast<IRoCSChannelSpecs*>(
      p_mainWidget->multiChannelModel()->channel(
          p_iRoCSChannelSelector->toStdString()));
}

void LayerAssignmentParametersDialog::setIRoCSChannel(
    IRoCSChannelSpecs *channel)
{
  if (p_iRoCSChannelSelector->contains(channel->name()))
      p_iRoCSChannelSelector->setValue(channel->name());
}

std::string LayerAssignmentParametersDialog::cacheFileName() const
{
  return p_cacheFileControlElement->value();
}

void LayerAssignmentParametersDialog::setCacheFileName(
    std::string const &cacheFileName)
{
  p_cacheFileControlElement->setValue(cacheFileName);
}

std::string LayerAssignmentParametersDialog::modelFileName() const
{
  return p_modelFileNameControlElement->value();
}

void LayerAssignmentParametersDialog::setModelFileName(
    std::string const &modelFileName)
{
  p_modelFileNameControlElement->setValue(modelFileName);
}

bool LayerAssignmentParametersDialog::cacheCoordinates() const
{
  return p_cacheCoordinates->value();
}

void LayerAssignmentParametersDialog::setCacheCoordinates(bool cache)
{
  p_cacheCoordinates->setValue(cache);
}

bool LayerAssignmentParametersDialog::updateMitoses() const
{
  return p_updateMitoses->value();
}

void LayerAssignmentParametersDialog::setUpdateMitoses(bool update)
{
  p_updateMitoses->setValue(update);
}

bool LayerAssignmentParametersDialog::forceFeatureComputation() const
{
  return p_forceFeatureComputationControl->value();
}

void LayerAssignmentParametersDialog::setForceFeatureComputation(bool force)
{
  p_forceFeatureComputationControl->setValue(force);
}

std::string LayerAssignmentParametersDialog::check()
{
  if (dataChannel() == NULL) return "The data channel has not been set.";
  if (annotationChannel() == NULL)
      return "The annotation channel has not been set.";

  if (annotationChannel()->markers().size() == 0)
      return "The annotation channel doesn't contain markers to be classified.";
  
  if (iRoCSChannel() == NULL) return "The iRoCS channel has not been set";

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

void LayerAssignmentParametersDialog::checkAndAccept()
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
        "iRoCSPipeline/AxisChannel",
        p_iRoCSChannelSelector->toStdString().c_str());
    settings.setValue(
        "iRoCSPipeline/LayerAssignmentModel",
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
        ("iRoCSPipeline/AxisChannel/" + p_mainWidget->fileName()).c_str(),
        p_iRoCSChannelSelector->toStdString().c_str());
    settings.setValue(
        ("iRoCSPipeline/LayerAssignmentModel/" +
         p_mainWidget->fileName()).c_str(),
        p_modelFileNameControlElement->toStdString().c_str());
    settings.setValue(
        ("iRoCSPipeline/CacheFile/" + p_mainWidget->fileName()).c_str(),
        p_cacheFileControlElement->toStdString().c_str());
  }
  
  accept();
}
