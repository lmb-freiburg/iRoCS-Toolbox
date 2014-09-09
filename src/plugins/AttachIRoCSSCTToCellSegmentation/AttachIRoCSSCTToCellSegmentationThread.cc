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
