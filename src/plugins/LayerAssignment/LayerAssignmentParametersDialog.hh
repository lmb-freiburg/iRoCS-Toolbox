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

#ifndef LAYERASSIGNMENTPARAMETERSDIALOG_HH
#define LAYERASSIGNMENTPARAMETERSDIALOG_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QDialog>

#include <liblabelling_qt4/ChannelSelectionControlElement.hh>
#include <liblabelling_qt4/FileNameSelectionControlElement.hh>
#include <liblabelling_qt4/BoolControlElement.hh>
#include <liblabelling_qt4/ChannelSpecs.hh>
#include <liblabelling_qt4/AnnotationChannelSpecs.hh>
#include <liblabelling_qt4/IRoCSChannelSpecs.hh>
#include <liblabelling_qt4/LabellingMainWidget.hh>

#include <string>

class LayerAssignmentParametersDialog :
    public QDialog
{

  Q_OBJECT

  public:
  
  LayerAssignmentParametersDialog(
      LabellingMainWidget* mainWidget,
      QWidget* parent = NULL, Qt::WindowFlags f = NULL);
  ~LayerAssignmentParametersDialog();

  ChannelSpecs *dataChannel() const;
  void setDataChannel(ChannelSpecs *channel);

  AnnotationChannelSpecs *annotationChannel() const;
  void setAnnotationChannel(AnnotationChannelSpecs *channel);

  IRoCSChannelSpecs *iRoCSChannel() const;
  void setIRoCSChannel(IRoCSChannelSpecs *channel);

  std::string cacheFileName() const;
  void setCacheFileName(std::string const &cacheFileName);

  std::string modelFileName() const;
  void setModelFileName(std::string const &modelFileName);

  bool cacheCoordinates() const;
  void setCacheCoordinates(bool cache);

  bool updateMitoses() const;
  void setUpdateMitoses(bool update);

  bool forceFeatureComputation() const;
  void setForceFeatureComputation(bool force);

  std::string check();

public slots:
  
  void checkAndAccept();

private:
  
  LabellingMainWidget* p_mainWidget;

  ChannelSelectionControlElement *p_dataChannelSelector;
  ChannelSelectionControlElement *p_annotationChannelSelector;
  ChannelSelectionControlElement *p_iRoCSChannelSelector;
  FileNameSelectionControlElement *p_cacheFileControlElement;
  FileNameSelectionControlElement *p_modelFileNameControlElement;
  BoolControlElement *p_cacheCoordinates;
  BoolControlElement *p_updateMitoses;
  BoolControlElement *p_forceFeatureComputationControl;

};

#endif
