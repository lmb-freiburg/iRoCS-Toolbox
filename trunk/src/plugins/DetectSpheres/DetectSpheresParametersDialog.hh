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

#ifndef DETECTSPHERESPARAMETERSDIALOG_HH
#define DETECTSPHERESPARAMETERSDIALOG_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QDialog>

#include <liblabelling_qt4/ChannelSpecs.hh>
#include <liblabelling_qt4/ChannelSelectionControlElement.hh>
#include <liblabelling_qt4/StringControlElement.hh>
#include <liblabelling_qt4/DoubleRangeControlElement.hh>
#include <liblabelling_qt4/DoubleControlElement.hh>
#include <liblabelling_qt4/LabellingMainWidget.hh>
#include <liblabelling_qt4/BoolControlElement.hh>

#include <string>

class DetectSpheresParametersDialog : public QDialog
{

  Q_OBJECT

  public:
  
  DetectSpheresParametersDialog(
      LabellingMainWidget* mainWidget,
      QWidget* parent = NULL, Qt::WindowFlags f = NULL);
  ~DetectSpheresParametersDialog();

  ChannelSpecs* dataChannel() const;
  void setDataChannel(ChannelSpecs *channel);

  std::string annotationChannelName() const;
  void setAnnotationChannelName(std::string const &name);

  blitz::TinyVector<double,2> radiusRangeUm() const;
  void setRadiusRangeUm(blitz::TinyVector<double,2> const &range);
  
  double radiusStepUm() const;
  void setRadiusStepUm(double radiusStepUm);

  double preSmoothingSigmaUm() const;
  void setPreSmoothingSigmaUm(double sigmaUm);

  double postSmoothingSigmaUm() const;
  void setPostSmoothingSigmaUm(double sigmaUm);

  double minMagnitude() const;
  void setMinMagnitude(double minMagnitude);

  bool invertGradients() const;
  void setInvertGradients(bool invert);

  double gamma() const;
  void setGamma(double gamma);

  std::string check();

public slots:
  
  void checkAndAccept();

private:
  
  LabellingMainWidget* p_mainWidget;

  ChannelSelectionControlElement *p_dataChannelSelector;
  StringControlElement *p_annotationChannelControlElement;
  DoubleRangeControlElement *p_radiusRangeControl;
  DoubleControlElement *p_radiusStepControl;
  DoubleControlElement *p_preSmoothingSigmaUmControl;
  DoubleControlElement *p_postSmoothingSigmaUmControl;
  DoubleControlElement *p_minMagnitudeControl;
  BoolControlElement *p_invertGradientsControl;
  DoubleControlElement *p_gammaControl;

};

#endif
