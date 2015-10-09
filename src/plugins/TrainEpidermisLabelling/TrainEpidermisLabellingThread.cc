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

#include "TrainEpidermisLabellingThread.hh"

#include <libIRoCS/TrainEpidermisLabellingWorker.hh>

TrainEpidermisLabellingThread::TrainEpidermisLabellingThread(
    TrainEpidermisLabellingParametersDialog const &parameters,
    iRoCS::ProgressReporter *progress) 
        : QThread(), _parameters(parameters), p_progress(progress),
          _sigmaMin(0.5), _sigmaMax(64.0), _sigmaStep(2.0), _bandMax(5)
{}

TrainEpidermisLabellingThread::~TrainEpidermisLabellingThread()
{}

void TrainEpidermisLabellingThread::run()
{
  iRoCS::trainEpidermisLabelling(_parameters, p_progress);
}
