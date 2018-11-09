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

#ifndef CONVERTMASKSTOMARKERSPARAMETERS_HH
#define CONVERTMASKSTOMARKERSPARAMETERS_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QDialog>

#include <liblabelling_qt4/Marker.hh>

class LabellingMainWidget;
class VisualizationChannelSpecs;
class ChannelSelectionControlElement;
class StringControlElement;
class StringSelectionControlElement;
class DoubleControlElement;
class BoolControlElement;
class DoubleRangeControlElement;
class PluginConvertMasksToMarkers;

class ConvertMasksToMarkersParameters : public QDialog
{

  Q_OBJECT

  public:

  ConvertMasksToMarkersParameters(LabellingMainWidget* mainWidget,
                                  QWidget* parent = 0, Qt::WindowFlags f = 0);
  ~ConvertMasksToMarkersParameters();

  VisualizationChannelSpecs *maskChannel() const;
  std::string annotationChannelName() const;
  bool doConnectedComponentLabelling() const;
  Marker::MarkerType markerType() const;
  double minimumSizePx() const;
  double maximumSizePx() const;
  double smoothingSigmaPx() const;
  double simplifyToleranceUm3() const;

protected slots:

  void checkAndAccept();

private:

  LabellingMainWidget* p_mainWidget;

  ChannelSelectionControlElement* p_maskChannelSelector;
  StringControlElement* p_annotationChannelControlElement;
  BoolControlElement *p_doConnectedComponentLabellingControlElement;
  StringSelectionControlElement* p_markerTypeControlElement;
  DoubleRangeControlElement *p_volumeRangePxControl;
  DoubleControlElement *p_smoothingSigmaPxControl;
  DoubleControlElement *p_simplifyToleranceUm3Control;

  friend class PluginConvertMasksToMarkers;

};

#endif
