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

#include "DetectNucleiParametersDialog.hh"

#include <QtGui/QDialogButtonBox>
#include <QtGui/QMessageBox>
#include <QtCore/QSettings>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPushButton>

#include <liblabelling_qt4/MultiChannelModel.hh>

#include <libsvmtl/StDataHdf5.hh>

#include <fstream>

DetectNucleiParametersDialog::DetectNucleiParametersDialog(
    LabellingMainWidget* mainWidget, QWidget* parent, Qt::WindowFlags f)
        : QDialog(parent, f), p_mainWidget(mainWidget), _memoryLimit(0)
{
  QVBoxLayout* mainLayout = new QVBoxLayout;

  QFrame* parameterPanel = new QFrame;
  parameterPanel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  QVBoxLayout* parameterLayout = new QVBoxLayout;
  
  p_dataChannelSelector = new ChannelSelectionControlElement(
      tr("Data channel:"), p_mainWidget->multiChannelModel(),
      ChannelSpecs::Data | ChannelSpecs::Visualization);
  parameterLayout->addWidget(p_dataChannelSelector);

  p_annotationChannelControlElement = new StringControlElement(
      tr("Annotation channel:"));
  parameterLayout->addWidget(p_annotationChannelControlElement);

  p_modelFileNameControlElement = new FileNameSelectionControlElement(
      tr("Model File:"), "", true, "HDF5 (*.h5)");
  parameterLayout->addWidget(p_modelFileNameControlElement);
  
  p_cacheFileControlElement = new FileNameSelectionControlElement(
      tr("Cache File:"), "", false, "HDF5 (*.h5)");
  parameterLayout->addWidget(p_cacheFileControlElement);  
  
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
        "iRoCSPipeline/AnnotationChannel", "/annotation/detector").toString();
    modelName = settings.value(
        "iRoCSPipeline/DetectorModel", "").toString();
    cacheName = "";
  }
  else
  {
    channelName = settings.value(
        ("iRoCSPipeline/DataChannel/" + p_mainWidget->fileName()).c_str(),
        p_dataChannelSelector->toStdString().c_str()).toString();
    annotationName = settings.value(
        ("iRoCSPipeline/AnnotationChannel/" + p_mainWidget->fileName()).c_str(),
        "annotation/detector").toString();
    modelName = settings.value(
        ("iRoCSPipeline/DetectorModel/" + p_mainWidget->fileName()).c_str(),
        settings.value(
            "iRoCSPipeline/DetectorModel", "").toString()).toString();
    cacheName = settings.value(
        ("iRoCSPipeline/CacheFile/" + p_mainWidget->fileName()).c_str(),
        "").toString();
  }
  if (p_dataChannelSelector->contains(channelName.toStdString()))
      p_dataChannelSelector->setValue(channelName.toStdString());
  p_annotationChannelControlElement->setValue(annotationName.toStdString());
  p_modelFileNameControlElement->setValue(modelName.toStdString());
  p_cacheFileControlElement->setValue(cacheName.toStdString());
  
  setLayout(mainLayout);

  if (p_mainWidget != NULL)
  {
    std::cout << "Plugin runs with memory limit of "
              << p_mainWidget->memoryLimit() / 1024 / 1024 << " MB"
              << std::endl;
    setMemoryLimit(p_mainWidget->memoryLimit());
  }
}

DetectNucleiParametersDialog::~DetectNucleiParametersDialog()
{}

ChannelSpecs* DetectNucleiParametersDialog::dataChannel() const
{
  return p_dataChannelSelector->selectedChannel();
}

void DetectNucleiParametersDialog::setDataChannel(ChannelSpecs *channel)
{
  if (p_dataChannelSelector->selectedChannel() == channel ||
      !p_dataChannelSelector->contains(channel)) return;
  p_dataChannelSelector->setValue(channel);
}

std::string DetectNucleiParametersDialog::annotationChannelName() const
{
  return p_annotationChannelControlElement->value();
}

void DetectNucleiParametersDialog::setAnnotationChannelName(
    std::string const &name)
{
  p_annotationChannelControlElement->setValue(name);
}

std::string DetectNucleiParametersDialog::cacheFileName() const
{
  return p_cacheFileControlElement->value();
}

void DetectNucleiParametersDialog::setCacheFileName(
    std::string const &cacheFileName)
{
  p_cacheFileControlElement->setValue(cacheFileName);
}

std::string DetectNucleiParametersDialog::modelFileName() const
{
  return p_modelFileNameControlElement->value();
}

void DetectNucleiParametersDialog::setModelFileName(
    std::string const &modelFileName)
{
  p_modelFileNameControlElement->setValue(modelFileName);
}

ptrdiff_t DetectNucleiParametersDialog::memoryLimit() const
{
  return _memoryLimit;
}

void DetectNucleiParametersDialog::setMemoryLimit(ptrdiff_t memoryLimit)
{
  _memoryLimit = memoryLimit;
}

std::string DetectNucleiParametersDialog::check()
{
  if (dataChannel() == NULL) return "The data channel has not been set.";

  try
  {
    // Check syntax of annotationGroup string
    try 
    {
      std::string annotationGroup =
          BlitzH5File::simplifyGroupDescriptor(annotationChannelName());
      setAnnotationChannelName(annotationGroup);
    }
    catch (BlitzH5Error&)
    {
      return "The annotation channel name you entered is no valid hdf5 group "
          "descriptor.";
    }
    
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

      // Check whether it is a two-class model
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

void DetectNucleiParametersDialog::checkAndAccept()
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
        p_annotationChannelControlElement->toStdString().c_str());
    settings.setValue(
        "iRoCSPipeline/DetectorModel",
        p_modelFileNameControlElement->toStdString().c_str());
  }
  else
  {
    settings.setValue(
        ("iRoCSPipeline/DataChannel/" + p_mainWidget->fileName()).c_str(),
        p_dataChannelSelector->toStdString().c_str());
    settings.setValue(
        ("iRoCSPipeline/AnnotationChannel/" + p_mainWidget->fileName()).c_str(),
        p_annotationChannelControlElement->toStdString().c_str());
    settings.setValue(
        ("iRoCSPipeline/DetectorModel/" + p_mainWidget->fileName()).c_str(),
        p_modelFileNameControlElement->toStdString().c_str());
    settings.setValue(
        ("iRoCSPipeline/CacheFile/" + p_mainWidget->fileName()).c_str(),
        p_cacheFileControlElement->toStdString().c_str());
  }
  
  accept();
}
