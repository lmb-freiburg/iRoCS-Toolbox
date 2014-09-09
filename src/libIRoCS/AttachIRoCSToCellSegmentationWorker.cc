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
