#include "LayerAssignmentThread.hh"

#include <libIRoCS/LayerAssignmentWorker.hh>

#include <liblabelling_qt4/MultiChannelModel.hh>
#include <liblabelling_qt4/DataChannelSpecs.hh>
#include <liblabelling_qt4/VisualizationChannelSpecs.hh>
#include <liblabelling_qt4/ChannelSpecsRenderer.hh>
#include <liblabelling_qt4/NucleusMarker.hh>

LayerAssignmentThread::LayerAssignmentThread(
    LayerAssignmentParametersDialog const &parameters,
    iRoCS::ProgressReporter *progress,
    LabellingMainWidget* mainWidget,
    QWidget* parent) 
        : QThread(parent), _parameters(parameters), p_progress(progress),
          p_mainWidget(mainWidget)
{}

LayerAssignmentThread::~LayerAssignmentThread()
{}

void LayerAssignmentThread::run()
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
  for (std::vector<Marker*>::const_iterator it =
           _parameters.annotationChannel()->begin();
       it != _parameters.annotationChannel()->end(); ++it)
  {
    atb::Nucleus nc;
    nuclei.push_back(nc);
    (*it)->copyToATBNucleus(nuclei.back());
  }
  
  iRoCS::assignLayers(
      data, nuclei, *_parameters.iRoCSChannel()->rct(),
      _parameters.modelFileName(), _parameters.cacheFileName(),
      _parameters.updateMitoses(), _parameters.cacheCoordinates(),
      _parameters.forceFeatureComputation(), p_progress);

  int i = 0;
  for (std::vector<Marker*>::const_iterator it =
           _parameters.annotationChannel()->begin();
       it != _parameters.annotationChannel()->end(); ++it, ++i)
      (*it)->copyFromATBNucleus(nuclei[i]);  
}