#include "DetectSpheresThread.hh"

#include <libIRoCS/DetectSpheresWorker.hh>

#include <liblabelling_qt4/NucleusMarker.hh>
#include <liblabelling_qt4/VisualizationChannelSpecs.hh>

DetectSpheresThread::DetectSpheresThread(
    DetectSpheresParametersDialog const &parameters,
    AnnotationChannelSpecs &markerChannel,
    iRoCS::ProgressReporter *progress, 
    LabellingMainWidget *mainWidget,
    DataChannelSpecs *responseChannel,
    DataChannelSpecs *radiusUmChannel,
    QWidget *parent) 
        : QThread(parent), _parameters(parameters),
          _markerChannel(markerChannel), p_progress(progress),
          p_mainWidget(mainWidget),
          p_responseChannel(responseChannel), 
          p_radiusUmChannel(radiusUmChannel)
{}

DetectSpheresThread::~DetectSpheresThread()
{}

void DetectSpheresThread::run()
{
  if (p_progress != NULL)
  {
    p_progress->updateProgressMessage(
        "Copying data channel to processing buffer");
    if (!p_progress->updateProgress(0)) return;
  }
  
  atb::Array<float,3> *data;
  if (_parameters.dataChannel()->channelType() == ChannelSpecs::Data)
  {
    data = static_cast<DataChannelSpecs*>(
        _parameters.dataChannel())->data();
  }
  else if (_parameters.dataChannel()->channelType() ==
           ChannelSpecs::Visualization)
  {
    VisualizationChannelSpecs *channel =
        static_cast<VisualizationChannelSpecs*>(_parameters.dataChannel());
    data = new atb::Array<float,3>(
        channel->data()->shape(), channel->data()->elementSizeUm(),
        channel->data()->transformation());
#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(data->size()); ++i)
        data->data()[i] = static_cast<float>(channel->data()->data()[i]);
  }
  else
  {
    p_progress->abortWithError("Invalid channel type given");
    return;
  }
  
  std::vector<atb::Nucleus> spheres;
  atb::Array<float,3> *response = (p_responseChannel != NULL) ?
      p_responseChannel->data() : new atb::Array<float,3>();
  atb::Array<float,3> *radiusUm = (p_radiusUmChannel != NULL) ?
      p_radiusUmChannel->data() : new atb::Array<float,3>();
  
  iRoCS::detectSpheres(
      *data, spheres, *response, *radiusUm, _parameters.radiusRangeUm(),
      _parameters.radiusStepUm(), _parameters.preSmoothingSigmaUm(),
      _parameters.postSmoothingSigmaUm(), _parameters.minMagnitude(),
      _parameters.invertGradients(), _parameters.gamma(), p_progress);
  
  if (p_responseChannel == NULL) delete response;
  if (p_radiusUmChannel == NULL) delete radiusUm;
  if (_parameters.dataChannel()->channelType() == ChannelSpecs::Visualization)
      delete data;

  /*---------------------------------------------------------------------
   *  Separate the dataset transformation
   *---------------------------------------------------------------------*/
  for (size_t i = 0; i < spheres.size(); ++i)
      spheres[i].setPositionUm(
          atb::homogeneousToEuclidean(
              _parameters.dataChannel()->transformation() *
              atb::euclideanToHomogeneous(
                  spheres[i].positionUm())));
  
  // Generate markers
  if (p_progress != NULL && !p_progress->updateProgressMessage(
          "Populating Annotation Channel")) return;
  _markerChannel.setTransformation(_parameters.dataChannel()->transformation());
  for (size_t i = 0; i < spheres.size(); ++i)
  {
    NucleusMarker *marker = new NucleusMarker();
    marker->setPositionUm(spheres[i].positionUm());
    marker->setRadiusUm(spheres[i].radiusUm());
    marker->setValue(spheres[i].value());
    _markerChannel.addMarker(marker);
  }
}

