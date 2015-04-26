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

#include "MarkerPresetWidget.hh"

#include <QtGui/QVBoxLayout>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QPushButton>
#include <QtGui/QScrollArea>
#include <QtGui/QColorDialog>

#include "Marker.hh"
#include "AnnotationChannelSpecs.hh"
#include "IntControlElement.hh"

MarkerPresetWidget::MarkerPresetWidget(
    AnnotationChannelSpecs* channel, QWidget* parent)
        : QGroupBox(tr("Marker Presets"), parent), p_channel(channel)
{
  // Preamble for setting up a GroupBox with scrollarea
  setFlat(true);
  setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
  p_layout = new QVBoxLayout;
  p_layout->setSpacing(2);
  QWidget* mainWidget = new QWidget;
  mainWidget->setLayout(p_layout);
  QScrollArea* scroller = new QScrollArea;
  scroller->setWidget(mainWidget);
  scroller->setWidgetResizable(true);
  QVBoxLayout* groupLayout = new QVBoxLayout;
  groupLayout->addWidget(scroller);
  setLayout(groupLayout);

  // The content
  QHBoxLayout* labelLayout = new QHBoxLayout;
  p_labelControl = new IntControlElement(tr("Label:"), -1);
  p_labelControl->setSwitchable(true);
  p_labelControl->setRange(-1, INT_MAX);
  p_labelControl->setSpecialValueText(tr("Unspecified"));
  labelLayout->addWidget(p_labelControl);
  p_alphaSpinBox = new QDoubleSpinBox;
  p_alphaSpinBox->setRange(0.0, 1.0);
  p_alphaSpinBox->setSingleStep(0.01);
  p_alphaSpinBox->setValue(1.0);
  labelLayout->addWidget(p_alphaSpinBox);
  connect(p_alphaSpinBox, SIGNAL(valueChanged(double)),
          SLOT(updateAlpha()));
  p_labelColorPushButton = new QPushButton;
  updateLabelColorPushButton();
  connect(p_labelControl, SIGNAL(valueChanged()),
          SLOT(updateLabelColorPushButton()));
  connect(p_labelColorPushButton, SIGNAL(clicked()), SLOT(chooseColor()));
  p_labelColorPushButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  labelLayout->addWidget(p_labelColorPushButton);
  p_layout->addLayout(labelLayout);
}

MarkerPresetWidget::~MarkerPresetWidget()
{}

void MarkerPresetWidget::getValues(Marker* marker) const
{
  if (marker != NULL && p_labelControl->isActive())
      marker->setLabel(p_labelControl->value());
}

void MarkerPresetWidget::setValues(const Marker* marker)
{
  if (marker != NULL) p_labelControl->setValue(marker->label());
}

void MarkerPresetWidget::updateAlpha()
{
  p_channel->setAlphaForLabel(
      p_labelControl->value(), static_cast<float>(p_alphaSpinBox->value()));
  updateLabelColorPushButton();
  emit colormapChanged();
}

void MarkerPresetWidget::chooseColor() 
{
  blitz::TinyVector<double,3> col =
      p_channel->colorForLabel(p_labelControl->value());  
  QColor color = QColorDialog::getColor(
      QColor(static_cast<int>(col(0) * 255.0),
             static_cast<int>(col(1) * 255.0),
             static_cast<int>(col(2) * 255.0)),
      this);
  if (color.isValid()) 
  {
    p_channel->setColorForLabel(
        p_labelControl->value(),
        blitz::TinyVector<double,3>(
            static_cast<double>(color.redF()),
            static_cast<double>(color.greenF()),
            static_cast<double>(color.blueF())));
    updateLabelColorPushButton();
  }
  emit colormapChanged();
}

void MarkerPresetWidget::updateLabelColorPushButton() 
{
  blitz::TinyVector<double,3> col(
      p_channel->colorForLabel(p_labelControl->value()));
  int alpha = static_cast<int>(
      255.0f * p_channel->alphaForLabel(p_labelControl->value()));
  if (p_alphaSpinBox->value() != static_cast<double>(
          p_channel->alphaForLabel(p_labelControl->value())))
      p_alphaSpinBox->setValue(
          static_cast<double>(
              p_channel->alphaForLabel(p_labelControl->value())));
  QPixmap pixmap(16, 16);
  pixmap.fill(QColor(static_cast<int>(col(0) * 255.0),
                     static_cast<int>(col(1) * 255.0),
                     static_cast<int>(col(2) * 255.0),
                     alpha));
  p_labelColorPushButton->setIcon(QIcon(pixmap));
}

