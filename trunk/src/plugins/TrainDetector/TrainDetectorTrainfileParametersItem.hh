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

#ifndef TRAINDETECTORTRAINFILEPARAMETERSITEM_HH
#define TRAINDETECTORTRAINFILEPARAMETERSITEM_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QTreeWidgetItem>

#include <libIRoCS/TrainfileParameters.hh>

#include <liblabelling_qt4/FileNameSelectionControlElement.hh>
#include <liblabelling_qt4/HDF5SelectionControlElement.hh>
#include <liblabelling_qt4/LabellingMainWidget.hh>

#include <string>

class TrainDetectorParametersDialog;

class TrainDetectorTrainfileParametersItem
    : public QTreeWidgetItem, public TrainfileParameters
{

public:

  TrainDetectorTrainfileParametersItem(
      LabellingMainWidget *mainWidget, std::string const &filename);
  ~TrainDetectorTrainfileParametersItem();
  
  void setTrainFileName(std::string const &name);
  std::string trainFileName() const;
  
  void setDataChannelName(std::string const &name);
  std::string dataChannelName() const;

  void setAnnotationChannelName(std::string const &name);
  std::string annotationChannelName() const;

  void setCacheFileName(std::string const &name);
  std::string cacheFileName() const;

  std::string check(int &nPositiveSamples, int &nSamples);

private:
  
  LabellingMainWidget *p_mainWidget;

  FileNameSelectionControlElement *p_trainFileSelector;
  HDF5SelectionControlElement *p_dataChannelControlElement;
  HDF5SelectionControlElement *p_annotationChannelControlElement;
  FileNameSelectionControlElement *p_cacheFileSelector;
  
  friend class TrainDetectorParametersDialog;

};

#endif
