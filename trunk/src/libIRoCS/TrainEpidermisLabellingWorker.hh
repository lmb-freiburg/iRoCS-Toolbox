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

#ifndef TRAINEPIDERMISLABELLINGWORKER_HH
#define TRAINEPIDERMISLABELLINGWORKER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtCore/QThread>

#include <libProgressReporter/ProgressReporter.hh>

#include "TrainingParameters.hh"
#include "iRoCSFeatures.hh"

class TrainEpidermisLabellingWorker : public QThread
{

  Q_OBJECT

  public:
  
  TrainEpidermisLabellingWorker(
      TrainingParameters const &parameters,
      iRoCS::ProgressReporter *progress = NULL);
  ~TrainEpidermisLabellingWorker();

  void run();

private:

  TrainingParameters const &_parameters;
  iRoCS::ProgressReporter *p_progress;
  
  double _sigmaMin, _sigmaMax, _sigmaStep;
  int _bandMax;

  iRoCS::Features *p_features;
  AnnotationChannelSpecs *p_annotation;

};

#endif
