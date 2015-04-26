/**************************************************************************
 *
 * This file belongs to the iRoCS Toolbox.
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

#include "MarkerControlWidget.hh"

#include <QtGui/QVBoxLayout>
#include <QtGui/QScrollArea>

#include "Marker.hh"
#include "IntControlElement.hh"
#include "BoolControlElement.hh"
#include "StringControlElement.hh"
#include "Double3ControlElement.hh"
#include "DoubleVectorControlElement.hh"
#include "IntDoubleMapControlElement.hh"

#if defined HAVE_CONFIG_H && !defined PACKAGE
#include <config.hh>
#endif

#ifdef HAVE_BLITZ_V9
#include <blitz/tinyvec-et.h>
#endif

MarkerControlWidget::MarkerControlWidget(
    AnnotationChannelSpecs* channel, QWidget* parent) 
        : QGroupBox(tr("Marker Control"), parent), p_channel(channel)
{
  // Preamble for setting up a GroupBox with scrollarea
  setFlat(true);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
  p_layout = new QVBoxLayout;
  p_layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
  p_layout->setSpacing(2);
  p_mainWidget = new QWidget;
  p_mainWidget->setLayout(p_layout);

  // The content
  p_showReadOnlyControl = new BoolControlElement(tr("Show All:"));
  connect(p_showReadOnlyControl, SIGNAL(valueChanged()),
          SLOT(toggleReadOnly()));
  p_layout->addWidget(p_showReadOnlyControl);

  p_idControl = new StringControlElement(tr("Id:"));
  p_idControl->setEnabled(false);
  p_layout->addWidget(p_idControl);
  
  p_positionControl = new Double3ControlElement(tr("Position:"));
  p_positionControl->setSingleStep(blitz::TinyVector<double,3>(0.1, 0.1, 0.1));
  connect(p_positionControl, SIGNAL(valueChanged()), SIGNAL(contentsChanged()));
  p_layout->addWidget(p_positionControl);

  p_iRoCSCoordinateControl = new Double3ControlElement(tr("Position (iRoCS):"));
  p_iRoCSCoordinateControl->setEnabled(false);
  p_layout->addWidget(p_iRoCSCoordinateControl);

  p_labelControl = new IntControlElement(tr("Label:"));
  p_labelControl->setRange(-1, INT_MAX);
  p_labelControl->setSpecialValueText(tr("Unspecified"));
  connect(p_labelControl, SIGNAL(valueChanged()), SIGNAL(contentsChanged()));
  p_layout->addWidget(p_labelControl);
  
  p_predictedLabelControl = new IntControlElement(tr("Predicted Label:"));
  p_predictedLabelControl->setRange(-1, INT_MAX);
  p_predictedLabelControl->setSpecialValueText(tr("Unspecified"));
  p_predictedLabelControl->setEnabled(false);
  p_layout->addWidget(p_predictedLabelControl);
  
  p_manualControl = new BoolControlElement(tr("Manual:"), true);
  p_manualControl->setEnabled(false);
  p_layout->addWidget(p_manualControl);
  
  p_needsFeatureUpdateControl = new BoolControlElement(
      tr("Needs Feature Update:"), true);
  p_needsFeatureUpdateControl->setEnabled(false);
  p_layout->addWidget(p_needsFeatureUpdateControl);
  
  p_featuresControl = new DoubleVectorControlElement(
      tr("Features:"), std::vector<double>());
  p_featuresControl->setEnabled(false);
  p_layout->addWidget(p_featuresControl);

  p_probabilitiesControl = new IntDoubleMapControlElement(
      tr("Platt Probabilities:"), std::map<int,double>());
  p_probabilitiesControl->setEnabled(false);
  p_probabilitiesControl->setRange(0.0, 1.0);
  p_layout->addWidget(p_probabilitiesControl);

  p_idControl->setVisible(p_showReadOnlyControl->value());
  p_predictedLabelControl->setVisible(p_showReadOnlyControl->value());
  p_manualControl->setVisible(p_showReadOnlyControl->value());
  p_needsFeatureUpdateControl->setVisible(p_showReadOnlyControl->value());
  p_featuresControl->setVisible(p_showReadOnlyControl->value());
  p_probabilitiesControl->setVisible(p_showReadOnlyControl->value());

  // Embed it into the scroll area
  p_scroller = new QScrollArea;
  p_scroller->setWidgetResizable(true);
  p_scroller->setWidget(p_mainWidget);
  QVBoxLayout* groupLayout = new QVBoxLayout;
  groupLayout->addWidget(p_scroller);
  setLayout(groupLayout);
}

MarkerControlWidget::~MarkerControlWidget()
{}

void MarkerControlWidget::getValues(Marker* marker) const
{
  marker->setPositionUm(p_positionControl->value());
  marker->setLabel(p_labelControl->value());
}

void MarkerControlWidget::resizeFeatures(int newSize)
{
  if (p_featuresControl->length() == static_cast<size_t>(newSize)) return;
  p_featuresControl->setLength(newSize);
}

size_t MarkerControlWidget::featuresSize() const
{
  return p_featuresControl->length();
}

void MarkerControlWidget::setValues(const Marker* marker)
{
  if (p_idControl->value() != marker->id()) p_idControl->setValue(marker->id());

  if (blitz::any(p_positionControl->value() != marker->positionUm()))
  {
    disconnect(p_positionControl, SIGNAL(valueChanged()),
               this, SIGNAL(contentsChanged()));
    p_positionControl->setValue(marker->positionUm());
    connect(p_positionControl, SIGNAL(valueChanged()),
            SIGNAL(contentsChanged()));
  }

  if (p_labelControl->value() != marker->label())
  {  
    disconnect(p_labelControl, SIGNAL(valueChanged()),
               this, SIGNAL(contentsChanged()));
    p_labelControl->setValue(marker->label());
    connect(p_labelControl, SIGNAL(valueChanged()), SIGNAL(contentsChanged()));
  }

  if (p_predictedLabelControl->value() != marker->predictedLabel())
      p_predictedLabelControl->setValue(marker->predictedLabel());

  if (p_manualControl->value() != marker->manual())
      p_manualControl->setValue(marker->manual());

  if (p_needsFeatureUpdateControl->value() != marker->needsFeatureUpdate())
      p_needsFeatureUpdateControl->setValue(marker->needsFeatureUpdate());

  p_featuresControl->setValue(marker->features());

  p_probabilitiesControl->setValue(marker->probabilityMap());

  if (marker->qcDistanceUm() != p_iRoCSCoordinateControl->value()(0) ||
      marker->radialDistanceUm() != p_iRoCSCoordinateControl->value()(1) ||
      marker->phi() != p_iRoCSCoordinateControl->value()(2))
      p_iRoCSCoordinateControl->setValue(
          blitz::TinyVector<double,3>(
              marker->qcDistanceUm(), marker->radialDistanceUm(),
              marker->phi()));
}

void MarkerControlWidget::toggleReadOnly()
{
  p_idControl->setVisible(p_showReadOnlyControl->value());
  p_predictedLabelControl->setVisible(p_showReadOnlyControl->value());
  p_manualControl->setVisible(p_showReadOnlyControl->value());
  p_needsFeatureUpdateControl->setVisible(p_showReadOnlyControl->value());
  p_featuresControl->setVisible(p_showReadOnlyControl->value());
  p_probabilitiesControl->setVisible(p_showReadOnlyControl->value());
  p_mainWidget->adjustSize();
}

