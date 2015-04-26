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

#include "AttachIRoCSSCTToCellSegmentationThread.hh"
#include "AttachIRoCSSCTToCellSegmentationParametersDialog.hh"

#include <libIRoCS/AttachIRoCSSCTToCellSegmentationWorker.hh>

#include <liblabelling_qt4/VisualizationChannelSpecs.hh>

AttachIRoCSSCTToCellSegmentationThread::AttachIRoCSSCTToCellSegmentationThread(
    AttachIRoCSSCTToCellSegmentationParametersDialog const &parameters,
    ShellCoordinateTransform *sct, iRoCS::ProgressReporter *progress,
    LabellingMainWidget *mainWidget, QWidget *parent) 
        : QThread(parent), _parameters(parameters), p_sct(sct),
          p_progress(progress), p_mainWidget(mainWidget)
{
  std::cout << "Initializing Attach iRoCS Shell Coordinate Transform to cell "
            << "segmentation... OK" << std::endl;
}

AttachIRoCSSCTToCellSegmentationThread::
~AttachIRoCSSCTToCellSegmentationThread()
{
  std::cout << "Cleaning up Attach iRoCS Shell coordinate transform to cell "
            << "segmentation... OK" << std::endl;
}

void AttachIRoCSSCTToCellSegmentationThread::run()
{
  std::cout << "Running AttachIRoCSSCTToCellSegmentationThread with "
            << "parameters: " << std::endl;
  std::cout << "  segmentation Channel = "
            << _parameters.segmentationChannel()->name() << std::endl;
  std::cout << "  QC Channel = " << _parameters.qcChannel()->name()
            << " (" << _parameters.qcChannel()->markers().size()
            << " markers)" << std::endl;
  std::cout << "  iRoCS Channel = " << _parameters.iRoCSChannelName()
            << std::endl;
  std::cout << "  background label = " << _parameters.backgroundLabel()
            << std::endl;
  std::cout << "  segment length [um] = " << _parameters.segmentLengthUm()
            << std::endl;
  std::cout << "  margin [um] = " << _parameters.marginUm() << std::endl;
  std::cout << "  downsample ratio = " << _parameters.downsampleRatio()
            << std::endl;
  
  iRoCS::attachIRoCS(
      *p_sct, *_parameters.segmentationChannel()->data(),
      _parameters.backgroundLabel(), _parameters.downsampleRatio(),
      _parameters.segmentLengthUm(), _parameters.marginUm(),
      _parameters.qcChannel()->markers().front()->positionUm(),
      blitz::TinyVector<double,3>(-std::numeric_limits<double>::infinity()),
      "", NULL, NULL, NULL, p_progress);

  if (p_progress != NULL)
      p_progress->updateProgress(p_progress->taskProgressMax());
}
