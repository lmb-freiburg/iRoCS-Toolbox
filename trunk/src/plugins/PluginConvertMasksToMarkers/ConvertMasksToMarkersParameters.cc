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

#include "ConvertMasksToMarkersParameters.hh"

#include <QtGui/QDialogButtonBox>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

#include <cassert>
#include <fstream>

#include <liblabelling_qt4/LabellingMainWidget.hh>
#include <liblabelling_qt4/VisualizationChannelSpecs.hh>
#include <liblabelling_qt4/MultiChannelModel.hh>
#include <liblabelling_qt4/StringControlElement.hh>
#include <liblabelling_qt4/ChannelSelectionControlElement.hh>
#include <liblabelling_qt4/StringSelectionControlElement.hh>
#include <liblabelling_qt4/DoubleControlElement.hh>
#include <liblabelling_qt4/BoolControlElement.hh>

ConvertMasksToMarkersParameters::ConvertMasksToMarkersParameters(
    LabellingMainWidget* mainWidget, QWidget* parent, Qt::WindowFlags f)
        : QDialog(parent, f), p_mainWidget(mainWidget)
{
  assert(mainWidget != NULL);
  QVBoxLayout* mainLayout = new QVBoxLayout;

  QFrame* parameterPanel = new QFrame;
  parameterPanel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  QVBoxLayout* parameterLayout = new QVBoxLayout;
  
  p_maskChannelSelector = new ChannelSelectionControlElement(
      tr("Data channel:"), mainWidget->multiChannelModel(),
      ChannelSpecs::Visualization);
  parameterLayout->addWidget(p_maskChannelSelector);
  p_annotationChannelControlElement = new StringControlElement(
      tr("Annotation channel:"));
  p_annotationChannelControlElement->setValue("/annotation/segments");
  parameterLayout->addWidget(p_annotationChannelControlElement);

  p_doConnectedComponentLabellingControlElement = new BoolControlElement(
      tr("Connected Component Labelling"));
  p_doConnectedComponentLabellingControlElement->setValue(false);
  parameterLayout->addWidget(p_doConnectedComponentLabellingControlElement);  

  QStringList mtStrings;
  mtStrings << tr("Surface") << tr("Cell");
  p_markerTypeControlElement = new StringSelectionControlElement(
      tr("Marker type:"), mtStrings);
  p_markerTypeControlElement->setValue("Surface");
  parameterLayout->addWidget(p_markerTypeControlElement);

  p_smoothingSigmaPxControl = new DoubleControlElement(
      tr("Smoothing sigma [px]:"), 1.0);
  p_smoothingSigmaPxControl->setRange(0.0, 1000.0);
  p_smoothingSigmaPxControl->setSingleStep(0.1);
  parameterLayout->addWidget(p_smoothingSigmaPxControl);  

  p_simplifyToleranceUm3Control = new DoubleControlElement(
      tr("Simplify tolerance [um^3]"), 0.0);
  p_simplifyToleranceUm3Control->setRange(
      0.0, std::numeric_limits<double>::infinity());
  p_simplifyToleranceUm3Control->setSingleStep(0.01);
  parameterLayout->addWidget(p_simplifyToleranceUm3Control);

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

  setLayout(mainLayout);
}

ConvertMasksToMarkersParameters::~ConvertMasksToMarkersParameters()
{}

VisualizationChannelSpecs* ConvertMasksToMarkersParameters::maskChannel() const
{
  return static_cast<VisualizationChannelSpecs*>(
      p_mainWidget->multiChannelModel()->channel(
          p_maskChannelSelector->toStdString()));
}

std::string ConvertMasksToMarkersParameters::annotationChannelName() const
{
  return p_annotationChannelControlElement->value();
}

bool ConvertMasksToMarkersParameters::doConnectedComponentLabelling() const
{
  return p_doConnectedComponentLabellingControlElement->value();
}

Marker::MarkerType ConvertMasksToMarkersParameters::markerType() const
{
  switch (p_markerTypeControlElement->value())
  {
  case 0:
    return Marker::Surface;
  case 1:
    return Marker::Cell;
  default:
    return Marker::Surface;
  }
}

double ConvertMasksToMarkersParameters::smoothingSigmaPx() const
{
  return p_smoothingSigmaPxControl->value();
}

double ConvertMasksToMarkersParameters::simplifyToleranceUm3() const
{
  return p_simplifyToleranceUm3Control->value();
}

void ConvertMasksToMarkersParameters::checkAndAccept()
{
  try
  {
    // Check syntax of annotationGroup string
    try 
    {
      std::string annotationGroup =
          BlitzH5File::simplifyGroupDescriptor(annotationChannelName());
      annotationGroup = annotationGroup.substr(0, annotationGroup.size() - 1);
      p_annotationChannelControlElement->setValue(annotationGroup);
    }
    catch (BlitzH5Error&)
    {
      QMessageBox::information(
          p_mainWidget, tr("Invalid annotation channel"),
          tr("The annotation channel name you entered is no valid hdf5 group "
             "descriptor."));
      return;
    }
  }
  catch (std::exception& e)
  {
    QMessageBox::information(
        p_mainWidget, tr("Parameter error"),
        tr("The supplied parameters contained an error: '") + e.what());
    return;
  }

  accept();
}
