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

#ifndef ATTACHIROCSPARAMETERSDIALOG_HH
#define ATTACHIROCSPARAMETERSDIALOG_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QDialog>

#include <liblabelling_qt4/AnnotationChannelSpecs.hh>
#include <liblabelling_qt4/ChannelSelectionControlElement.hh>
#include <liblabelling_qt4/StringControlElement.hh>
#include <liblabelling_qt4/IntControlElement.hh>
#include <liblabelling_qt4/DoubleControlElement.hh>
#include <liblabelling_qt4/LabellingMainWidget.hh>

class AttachIRoCSParametersDialog : public QDialog
{

  Q_OBJECT

  public:
  
  AttachIRoCSParametersDialog(
      LabellingMainWidget* mainWidget,
      QWidget* parent = NULL, Qt::WindowFlags f = NULL);
  ~AttachIRoCSParametersDialog();

  AnnotationChannelSpecs *markerChannel() const;
  void setMarkerChannel(AnnotationChannelSpecs *channel);

  AnnotationChannelSpecs *qcChannel() const;
  void setQcChannel(AnnotationChannelSpecs *channel);

  std::string iRoCSChannelName() const;
  void setIRoCSChannelName(std::string const &name);

  double dataWeight() const;
  void setDataWeight(double kappa);

  double axisStiffnessWeight() const;
  void setAxisStiffnessWeight(double lambda);

  double thicknessConstancyWeight() const;
  void setThicknessConstancyWeight(double mu);

  double searchRadiusUm() const;
  void setSearchRadiusUm(double searchRadiusUm);

  int nIterations() const;
  void setNIterations(int nIter);

  double optimizationTimeStep() const;
  void setOptimizationTimeStep(double tau);

  std::string check();

public slots:
  
  void checkAndAccept();

private:
  
  LabellingMainWidget *p_mainWidget;

  ChannelSelectionControlElement *p_markerChannelSelector;
  ChannelSelectionControlElement *p_qcChannelSelector;
  StringControlElement *p_iRoCSChannelNameControlElement;

  DoubleControlElement *p_kappaControl;
  DoubleControlElement *p_lambdaControl;
  DoubleControlElement *p_muControl;
  IntControlElement *p_nIterControl;
  DoubleControlElement *p_tauControl;
  DoubleControlElement *p_searchRadiusUmControl;

};

#endif
