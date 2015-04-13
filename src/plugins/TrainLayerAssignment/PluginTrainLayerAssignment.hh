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

#ifndef PLUGINTRAINLAYERASSIGNMENT_HH
#define PLUGINTRAINLAYERASSIGNMENT_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <liblabelling_qt4/PluginInterface.hh>

#include "TrainLayerAssignmentParametersDialog.hh"

#include <libIRoCS/TrainLayerAssignmentWorker.hh>

#include <QtCore/QtPlugin>

class PluginTrainLayerAssignment : public QObject, public PluginInterface
{

  Q_OBJECT
  Q_INTERFACES(PluginInterface)

public:

  PluginTrainLayerAssignment();
  ~PluginTrainLayerAssignment();
  
  QString name() const;
  void setLabellingMainWidget(LabellingMainWidget* parent);
  void setParameters(const std::map<std::string,std::string>& parameters);

  void run();

  void abort();

private:

  LabellingMainWidget* p_mainWidget;
  TrainLayerAssignmentParametersDialog* p_dialog;
  TrainLayerAssignmentWorker* p_workerThread;
  bool _interactive;

  iRoCS::ProgressReporterQt4 *p_progress;

};

#endif
