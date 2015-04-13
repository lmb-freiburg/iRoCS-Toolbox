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

#include "MitosisClassificationParametersDialog.hh"

#include <QtGui/QDialogButtonBox>
#include <QtGui/QMessageBox>
#include <QtCore/QSettings>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPushButton>

#include <liblabelling_qt4/MultiChannelModel.hh>

#include <libsvmtl/StDataHdf5.hh>

#include <fstream>

MitosisClassificationParametersDialog::MitosisClassificationParametersDialog(
    LabellingMainWidget* mainWidget, QWidget* parent, Qt::WindowFlags f)
        : QDialog(parent, f), MitosisClassificationParameters(),
          p_mainWidget(mainWidget)
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
        "iRoCSPipeline/MitosisClassificationModel", "").toString();
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
        ("iRoCSPipeline/MitosisClassificationModel/" +
         p_mainWidget->fileName()).c_str(),
        settings.value(
            "iRoCSPipeline/MitosisClassificationModel",
            "").toString()).toString();
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
  
  setLayout(mainLayout);
}

MitosisClassificationParametersDialog::~MitosisClassificationParametersDialog()
{}

DataChannelSpecs* MitosisClassificationParametersDialog::dataChannel() const
{
  return static_cast<DataChannelSpecs*>(
      p_mainWidget->multiChannelModel()->channel(
          p_dataChannelSelector->toStdString()));
}

void MitosisClassificationParametersDialog::setDataChannel(
    DataChannelSpecs *channel)
{
  if (p_dataChannelSelector->contains(channel->name()))
  {
    p_dataChannelSelector->setValue(channel->name());
  }
}

AnnotationChannelSpecs
*MitosisClassificationParametersDialog::annotationChannel() const
{
  return static_cast<AnnotationChannelSpecs*>(
      p_mainWidget->multiChannelModel()->channel(
          p_annotationChannelSelector->toStdString()));
}

void MitosisClassificationParametersDialog::setAnnotationChannel(
    AnnotationChannelSpecs *channel)
{
  if (p_annotationChannelSelector->contains(channel->name()))
  {
    p_annotationChannelSelector->setValue(channel->name());
  }
}

std::string MitosisClassificationParametersDialog::cacheFileName() const
{
  return p_cacheFileControlElement->value();
}

void MitosisClassificationParametersDialog::setCacheFileName(
    std::string const &cacheFileName)
{
  p_cacheFileControlElement->setValue(cacheFileName);
}

std::string MitosisClassificationParametersDialog::modelFileName() const
{
  return p_modelFileNameControlElement->value();
}

void MitosisClassificationParametersDialog::setModelFileName(
    std::string const &modelFileName)
{
  p_modelFileNameControlElement->setValue(modelFileName);
}

void MitosisClassificationParametersDialog::checkAndAccept()
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
        "iRoCSPipeline/MitosisClassificationModel",
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
        ("iRoCSPipeline/MitosisClassificationModel/" +
         p_mainWidget->fileName()).c_str(),
        p_modelFileNameControlElement->toStdString().c_str());
    settings.setValue(
        ("iRoCSPipeline/CacheFile/" + p_mainWidget->fileName()).c_str(),
        p_cacheFileControlElement->toStdString().c_str());
  }
  
  accept();
}
