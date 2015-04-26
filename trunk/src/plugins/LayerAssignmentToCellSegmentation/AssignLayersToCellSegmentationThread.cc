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

#include "AssignLayersToCellSegmentationThread.hh"
#include "AssignLayersToCellSegmentationParametersDialog.hh"

#include <libIRoCS/AssignLayersToCellSegmentationWorker.hh>

AssignLayersToCellSegmentationThread::AssignLayersToCellSegmentationThread(
    AssignLayersToCellSegmentationParametersDialog const &parameters,
    iRoCS::ProgressReporter *progress, LabellingMainWidget *mainWidget,
    QWidget *parent) 
        : QThread(parent), _parameters(parameters),
          p_progress(progress), p_mainWidget(mainWidget)
{
  std::cout << "Initializing Assign layers to cell segmentation... OK"
            << std::endl;
}

AssignLayersToCellSegmentationThread::~AssignLayersToCellSegmentationThread()
{
  std::cout << "Cleaning up Assign layers to cell segmentation... OK"
            << std::endl;
}

void AssignLayersToCellSegmentationThread::run()
{
  std::cout << "Running AssignLayersToCellSegmentationThread with "
            << "parameters: " << std::endl;
  std::cout << "  segmentation Channel = "
            << _parameters.segmentationChannel()->name() << std::endl;
  std::cout << "  iRoCS Channel = " << _parameters.iRoCSChannel()->name()
            << std::endl;
  std::cout << "  output file = " << _parameters.outFileName() << std::endl;
  std::cout << "  label dataset = " << _parameters.labelName() << std::endl;
  std::cout << "  model file = " << _parameters.modelFileName() << std::endl;
  std::cout << "  feature file = " << _parameters.featureFileName() << std::endl;
  std::cout << "  feature group = " << _parameters.featureGroup() << std::endl;
  std::cout << "  volume threshold [um^3] = "
            << _parameters.volumeThresholdUm3() << std::endl;
  std::cout << "  background label = " << _parameters.backgroundLabel()
            << std::endl;
  
  std::vector<std::string> featureNames;
  iRoCS::assignLayersToSegmentation(
      *_parameters.segmentationChannel()->data(),
      *_parameters.iRoCSChannel()->sct(),
      _parameters.volumeThresholdUm3(), _parameters.modelFileName(),
      _parameters.featureFileName(), _parameters.featureGroup(),
      featureNames, _parameters.outFileName(), _parameters.labelName(),
      _parameters.backgroundLabel(), p_progress);

  if (p_progress != NULL)
      p_progress->updateProgress(p_progress->taskProgressMax());
}
