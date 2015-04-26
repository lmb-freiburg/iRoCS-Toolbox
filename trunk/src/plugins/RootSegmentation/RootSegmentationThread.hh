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

#ifndef ROOTSEGMENTATIONTHREAD_HH
#define ROOTSEGMENTATIONTHREAD_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtCore/QThread>

#include <liblabelling_qt4/LabellingMainWidget.hh>
#include <liblabelling_qt4/VisualizationChannelSpecs.hh>

#include <libProgressReporter/ProgressReporter.hh>

#include "RootSegmentationParametersDialog.hh"

class RootSegmentationThread : public QThread
{

  Q_OBJECT

  public:
  
  RootSegmentationThread(
      RootSegmentationParametersDialog const &parameters,
      VisualizationChannelSpecs &segmentationChannel,
      iRoCS::ProgressReporter *progress = NULL,
      LabellingMainWidget *mainWidget = NULL, QWidget *parent = NULL);
  ~RootSegmentationThread();

  void run();

signals:
  
  void error(QString const &);

private:

  void _convertChannelData(
      ChannelSpecs const *channel, atb::Array<double,3> &out);

  RootSegmentationParametersDialog const &_parameters;
  VisualizationChannelSpecs &_segmentationChannel;
  iRoCS::ProgressReporter *p_progress;
  LabellingMainWidget* p_mainWidget;

};

#endif
