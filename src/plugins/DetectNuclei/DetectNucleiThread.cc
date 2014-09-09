#include "DetectNucleiThread.hh"

#include <libIRoCS/DetectNucleiWorker.hh>

#include <liblabelling_qt4/NucleusMarker.hh>
#include <liblabelling_qt4/DataChannelSpecs.hh>
#include <liblabelling_qt4/VisualizationChannelSpecs.hh>
#include <liblabelling_qt4/AnnotationChannelSpecs.hh>

DetectNucleiThread::DetectNucleiThread(
    DetectNucleiParametersDialog const &parameters,
    AnnotationChannelSpecs &markerChannel,
    iRoCS::ProgressReporter *progress, 
    LabellingMainWidget *mainWidget,
    QWidget *parent) 
        : QThread(parent), _parameters(parameters),
          _markerChannel(markerChannel), p_progress(progress),
          p_mainWidget(mainWidget)
{}

DetectNucleiThread::~DetectNucleiThread()
{}

void DetectNucleiThread::run()
{
  atb::Array<double,3> data;
  if (_parameters.dataChannel()->channelType() == ChannelSpecs::Data)
  {
    DataChannelSpecs *channel =
        static_cast<DataChannelSpecs*>(_parameters.dataChannel());
    data.resize(channel->data()->shape());
    data.setElementSizeUm(channel->data()->elementSizeUm());
    data = blitz::cast<double>(*channel->data());
  }
  else if (_parameters.dataChannel()->channelType() ==
           ChannelSpecs::Visualization)
  {
    VisualizationChannelSpecs *channel =
        static_cast<VisualizationChannelSpecs*>(_parameters.dataChannel());
    data.resize(channel->data()->shape());
    data.setElementSizeUm(channel->data()->elementSizeUm());
    data = blitz::cast<double>(*channel->data());
  }
  
  std::vector<atb::Nucleus> nuclei;

  detectNuclei(
      data, nuclei, _parameters.modelFileName(),
      _parameters.memoryLimit(), _parameters.cacheFileName(), p_progress);
  
  if (p_progress != NULL && !p_progress->updateProgressMessage(
          tr("Populating annotation channel").toStdString())) return;
  for (size_t i = 0; i < nuclei.size(); ++i)
  {
    NucleusMarker* m =
        new NucleusMarker(nuclei[i].positionUm(), nuclei[i].radiusUm());
    m->setValue(nuclei[i].value());
    _markerChannel.addMarker(m);
  }
}
