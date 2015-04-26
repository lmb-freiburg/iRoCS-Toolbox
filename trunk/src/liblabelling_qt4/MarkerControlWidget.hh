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

#ifndef MARKERCONTROLWIDGET_HH
#define MARKERCONTROLWIDGET_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QGroupBox>

class QVBoxLayout;
class QScrollArea;

class AnnotationChannelSpecs;
class Marker;
class BoolControlElement;
class IntControlElement;
class StringControlElement;
class Double3ControlElement;
class DoubleVectorControlElement;
class IntDoubleMapControlElement;

class MarkerControlWidget : public QGroupBox 
{

  Q_OBJECT

public:

  MarkerControlWidget(AnnotationChannelSpecs* channel, QWidget* parent = 0);
  virtual ~MarkerControlWidget();
  
  virtual void getValues(Marker* marker) const;
  size_t featuresSize() const;

public slots:

  void resizeFeatures(int newSize);
  virtual void setValues(const Marker* marker);

signals:
  
  void contentsChanged();

protected slots:
  
  virtual void toggleReadOnly();

protected:
  
  AnnotationChannelSpecs* p_channel;

  QWidget* p_mainWidget;
  QVBoxLayout* p_layout;
  QScrollArea* p_scroller;

  BoolControlElement* p_showReadOnlyControl;
  StringControlElement* p_idControl;
  Double3ControlElement* p_positionControl;
  IntControlElement* p_labelControl;
  IntControlElement* p_predictedLabelControl;
  BoolControlElement* p_manualControl;
  BoolControlElement *p_needsFeatureUpdateControl;
  DoubleVectorControlElement* p_featuresControl;
  IntDoubleMapControlElement* p_probabilitiesControl;
  Double3ControlElement *p_iRoCSCoordinateControl;

};

#endif
