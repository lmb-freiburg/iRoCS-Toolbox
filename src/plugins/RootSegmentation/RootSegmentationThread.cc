#include "RootSegmentationThread.hh"

#include <liblabelling_qt4/MultiChannelModel.hh>
#include <liblabelling_qt4/DataChannelSpecs.hh>
#include <liblabelling_qt4/CellMarker.hh>

#include <libIRoCS/RootSegmentationWorker.hh>

RootSegmentationThread::RootSegmentationThread(
    RootSegmentationParametersDialog const &parameters,
    VisualizationChannelSpecs &segmentationChannel,
    iRoCS::ProgressReporter *progress, 
    LabellingMainWidget *mainWidget, QWidget *parent) 
        : QThread(parent), _parameters(parameters),
          _segmentationChannel(segmentationChannel),
          p_progress(progress), p_mainWidget(mainWidget)
{}

RootSegmentationThread::~RootSegmentationThread()
{}

void RootSegmentationThread::_convertChannelData(
    ChannelSpecs const *channel, atb::Array<double,3> &out)
{
  switch (channel->channelType())
  {
  case ChannelSpecs::Data :
  {
    atb::Array<float,3> const *in =
        reinterpret_cast<DataChannelSpecs const*>(channel)->data();
    out.resize(in->shape());
    out.setElementSizeUm(in->elementSizeUm());
    out = blitz::cast<double>(*in);
    break;
  }
  case ChannelSpecs::Visualization :
  {
    atb::Array<int,3> const *in =
        reinterpret_cast<VisualizationChannelSpecs const*>(channel)->data();
    out.resize(in->shape());
    out.setElementSizeUm(in->elementSizeUm());
    out = blitz::cast<double>(*in);
    break;
  }
  default:
  {
    std::cerr << __FILE__ << ":" << __LINE__ << ": Unknown channel type..."
              << std::endl;
    exit(-1);
  }
  }
}

void RootSegmentationThread::run()
{
  // Copy channel data to double Array and retrieve elementSizeUm
  if (p_progress != NULL &&
      !p_progress->updateProgressMessage(
          "Cell Segmentation: Copying data...")) return;
  atb::Array<double,3> data;
  _convertChannelData(_parameters.dataChannel(), data);

  segmentCells(
      data, *_segmentationChannel.data(), _parameters.gamma(),
      _parameters.normalizationType(),
      _parameters.applyMedian() ? _parameters.medianWidthPx() : 0,
      _parameters.processingElementSizeUm(),
      _parameters.applyVarianceNormalization() ?
      _parameters.varSigmaUm() : 0.0, _parameters.epsilon(),
      _parameters.hessianSigmaUm(), _parameters.applyDiffusion(),
      _parameters.nDiffusionIterations(), _parameters.zCompensationFactor(),
      _parameters.kappa(), _parameters.tau(), _parameters.edgeThreshold(),
      _parameters.minimumCellVolumeUm3(), _parameters.boundaryThicknessPx(),
      _parameters.debugFileName(), p_progress);
}
