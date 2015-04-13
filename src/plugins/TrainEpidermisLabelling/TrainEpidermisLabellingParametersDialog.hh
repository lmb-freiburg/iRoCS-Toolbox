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

#ifndef TRAINEPIDERMISLABELLINGPARAMETERSDIALOG_HH
#define TRAINEPIDERMISLABELLINGPARAMETERSDIALOG_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QTreeWidget>

#include <libIRoCS/TrainingParameters.hh>

#include <liblabelling_qt4/FileNameSelectionControlElement.hh>
#include <liblabelling_qt4/HDF5SelectionControlElement.hh>
#include <liblabelling_qt4/StringControlElement.hh>
#include <liblabelling_qt4/StringSelectionControlElement.hh>
#include <liblabelling_qt4/IntControlElement.hh>
#include <liblabelling_qt4/DoubleControlElement.hh>
#include <liblabelling_qt4/LabellingMainWidget.hh>

class TrainfileParameters;

class TrainEpidermisLabellingParametersDialog
    : public QDialog, public TrainingParameters
{

  Q_OBJECT

public:
  
  TrainEpidermisLabellingParametersDialog(
      LabellingMainWidget* mainWidget,
      QWidget* parent = 0, Qt::WindowFlags f = 0);
  ~TrainEpidermisLabellingParametersDialog();

  std::vector<TrainfileParameters*> trainFiles() const;

  void setFeatureGroup(std::string const &group);
  std::string featureGroup() const;

  void setGenerateRandomSamples(bool generate);
  bool generateRandomSamples() const;

  void setNInRootSamples(int samples);
  int nInRootSamples() const;

  void setNOutRootSamples(int samples);
  int nOutRootSamples() const;

  void setModelFileName(std::string const &name);
  std::string modelFileName() const;

  void setSdFeatureNormalization(iRoCS::Features::NormalizationType normType);
  iRoCS::Features::NormalizationType sdFeatureNormalization() const;

  void setHoughFeatureNormalization(
      iRoCS::Features::NormalizationType normType);
  iRoCS::Features::NormalizationType houghFeatureNormalization() const;

  void setCost(double cost);
  double cost() const;

  void setGamma(double gamma);
  double gamma() const;

private slots:
  
  void checkAndAccept();
  void addTrainFile();

private:

  LabellingMainWidget* p_mainWidget;

  QTreeWidget *p_trainFileList;
  QPushButton *p_addTrainFileButton;
  StringControlElement *p_featureGroupControlElement;
  QGroupBox* p_randomSamplesGroup;
  IntControlElement* p_nInRootSamplesControlElement;
  IntControlElement* p_nOutRootSamplesControlElement;
  FileNameSelectionControlElement* p_modelFileNameControlElement;
  StringSelectionControlElement *p_sdNormalizationControl;
  StringSelectionControlElement *p_houghNormalizationControl;
  DoubleControlElement* p_costControlElement;
  DoubleControlElement* p_gammaControlElement;
  
};

#endif
