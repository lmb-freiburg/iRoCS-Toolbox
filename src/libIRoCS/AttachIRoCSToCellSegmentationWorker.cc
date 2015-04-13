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

#include "AttachIRoCSToCellSegmentationWorker.hh"

#include "AttachIRoCSToCellSegmentationParameters.hh"

#include <liblabelling_qt4/VisualizationChannelSpecs.hh>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#endif

AttachIRoCSToCellSegmentationWorker::AttachIRoCSToCellSegmentationWorker(
    AttachIRoCSToCellSegmentationParameters const &parameters,
    atb::IRoCS *rct, iRoCS::ProgressReporter *progress,
    LabellingMainWidget *mainWidget, QWidget *parent) 
        : QThread(parent), _parameters(parameters), p_rct(rct),
          p_progress(progress), p_mainWidget(mainWidget)
{
  std::cout << "Initializing Attach iRoCS to cell segmentation... OK"
            << std::endl;
}

AttachIRoCSToCellSegmentationWorker::~AttachIRoCSToCellSegmentationWorker()
{
  std::cout << "Cleaning up Attach iRoCS to cell segmentation... OK"
            << std::endl;
}

void AttachIRoCSToCellSegmentationWorker::run()
{
  std::cout << "Running AttachIRoCSToCellSegmentationWorker thread with "
            << "parameters: "
            << std::endl;
  std::cout << "  segmentation Channel = "
            << _parameters.segmentationChannel()->name() << std::endl;
  std::cout << "  QC Channel = " << _parameters.qcChannel()->name()
            << " (" << _parameters.qcChannel()->markers().size()
            << " markers)" << std::endl;
  std::cout << "  iRoCS Channel = " << _parameters.iRoCSChannelName()
            << std::endl;
  std::cout << "  dataWeight (kappa) = " << _parameters.dataWeight()
            << std::endl;
  std::cout << "  Axis stiffness weight (lambda) = "
            << _parameters.axisStiffnessWeight() << std::endl;
  std::cout << "  Thickness Constancy weight (mu) = "
            << _parameters.thicknessConstancyWeight() << std::endl;
  std::cout << "  nIterations = " << _parameters.nIterations() << std::endl;
  std::cout << "  Time step (tau) = " << _parameters.optimizationTimeStep()
            << std::endl;

  if (p_progress != NULL)
  {
    p_progress->setProgressMin(0);
    p_progress->setProgressMax(110);
    if (!p_progress->updateProgress(1)) return;
    if (!p_progress->updateProgressMessage("Attaching iRoCS...")) return;
  }

  p_rct->fit(
      (*_parameters.qcChannel()->begin())->positionUm(),
      *_parameters.segmentationChannel()->data(),
      _parameters.dataWeight(), _parameters.axisStiffnessWeight(),
      _parameters.thicknessConstancyWeight(), _parameters.nIterations(),
      _parameters.optimizationTimeStep());
}
