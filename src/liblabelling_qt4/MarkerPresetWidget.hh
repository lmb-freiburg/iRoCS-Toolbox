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

#ifndef MARKERPRESETWIDGET_HH
#define MARKERPRESETWIDGET_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QGroupBox>

class AnnotationChannelSpecs;
class Marker;
class IntControlElement;

class QVBoxLayout;
class QDoubleSpinBox;
class QPushButton;

class MarkerPresetWidget : public QGroupBox
{
  
  Q_OBJECT

public:

  MarkerPresetWidget(AnnotationChannelSpecs* channel, QWidget* parent = 0);
  virtual ~MarkerPresetWidget();

  virtual void getValues(Marker* marker) const;
  virtual void setValues(const Marker* marker);

signals:
  
  void colormapChanged();

protected slots:
  
  void updateAlpha();
  void chooseColor();
  void updateLabelColorPushButton();

protected:
  
  AnnotationChannelSpecs* p_channel;

  QVBoxLayout* p_layout;
  IntControlElement* p_labelControl;
  QDoubleSpinBox *p_alphaSpinBox;
  QPushButton* p_labelColorPushButton;

};

#endif
