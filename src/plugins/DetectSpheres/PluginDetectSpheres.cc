#include "PluginDetectSpheres.hh"
#include "DetectSpheresThread.hh"

#include <QtGui/QMessageBox>
#include <QtGui/QApplication>

#include <liblabelling_qt4/MultiChannelModel.hh>
#include <liblabelling_qt4/LabellingMainWidget.hh>
#include <liblabelling_qt4/ChannelSelectionControlElement.hh>
#include <liblabelling_qt4/FileNameSelectionControlElement.hh>
#include <liblabelling_qt4/AnnotationChannelSpecs.hh>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <unistd.h>
#endif

PluginDetectSpheres::PluginDetectSpheres() 
        : QObject(), p_mainWidget(NULL), p_dialog(NULL), _interactive(true),
          p_progress(NULL)
{}

PluginDetectSpheres::~PluginDetectSpheres()
{
  delete p_dialog;
}

QString PluginDetectSpheres::name() const
{
  return QString(tr("Detect Spheres"));
}

void PluginDetectSpheres::setLabellingMainWidget(LabellingMainWidget* parent)
{
  p_mainWidget = parent;
  delete p_dialog;
  p_dialog = new DetectSpheresParametersDialog(p_mainWidget, p_mainWidget);
}

void PluginDetectSpheres::setParameters(
    const std::map<std::string,std::string>& parameters)
{
  if (parameters.find("dataChannel") != parameters.end())
      p_dialog->setDataChannel(
          p_mainWidget->multiChannelModel()->channel(
              parameters.find("dataChannel")->second));

  if (parameters.find("annotationChannel") != parameters.end())
      p_dialog->setAnnotationChannelName(
          parameters.find("annotationChannel")->second);

  if (parameters.find("radiusMinUm") != parameters.end())
      p_dialog->setRadiusRangeUm(
          blitz::TinyVector<double,2>(
              atof(parameters.find("radiusMinUm")->second.c_str()),
              p_dialog->radiusRangeUm()(1)));
  
  if (parameters.find("radiusMaxUm") != parameters.end())
      p_dialog->setRadiusRangeUm(
          blitz::TinyVector<double,2>(
              p_dialog->radiusRangeUm()(0),
              atof(parameters.find("radiusMaxUm")->second.c_str())));
  
  if (parameters.find("radiusStepUm") != parameters.end())
      p_dialog->setRadiusStepUm(
          atof(parameters.find("radiusStepUm")->second.c_str()));

  if (parameters.find("preSmoothingSigmaUm") != parameters.end())
      p_dialog->setPreSmoothingSigmaUm(
          atof(parameters.find("preSmoothingSigmaUm")->second.c_str()));

  if (parameters.find("postSmoothingSigmaUm") != parameters.end())
      p_dialog->setPostSmoothingSigmaUm(
          atof(parameters.find("postSmoothingSigmaUm")->second.c_str()));

  if (parameters.find("minMagnitude") != parameters.end())
      p_dialog->setMinMagnitude(
          atof(parameters.find("minMagnitude")->second.c_str()));

  if (parameters.find("invertGradients") != parameters.end())
      p_dialog->setInvertGradients(
          parameters.find("invertGradients")->second == "1");

  if (parameters.find("gamma") != parameters.end())
      p_dialog->setGamma(atof(parameters.find("gamma")->second.c_str()));

  _interactive = false;
}

void PluginDetectSpheres::run()
{
  std::cout << "Running plugin " << name().toStdString()
            << " linked against "
            << LabellingMainWidget::__labellingVersionString << std::endl;

  if (p_mainWidget->__labellingVersionString !=
      LabellingMainWidget::__labellingVersionString)
  {
    std::cerr << "Warning: Plugin is linked against different liblabelling "
              << "library version: " << std::endl;
    std::cerr << "  Plugin version: "
              << LabellingMainWidget::__labellingVersionString << std::endl;
    std::cerr << "  labelling version: "
              << p_mainWidget->__labellingVersionString << std::endl;
  }

  // Check whether at least one Data or Visualization channel is loaded,
  // otherwise give up
  std::vector<ChannelSpecs*>::const_iterator it;
  for (it = p_mainWidget->multiChannelModel()->begin();
       it != p_mainWidget->multiChannelModel()->end(); ++it)
  {
    if ((*it)->channelType() == ChannelSpecs::Data ||
        (*it)->channelType() == ChannelSpecs::Visualization) break;
  }
  if (it == p_mainWidget->multiChannelModel()->end())
  {
    QMessageBox::warning(
        p_mainWidget, tr("Plugin prerequisites not met"),
        tr("This plugin needs one data or visualization channel to detect "
           "spheres in. Please load a data channel."));
    return;
  }
  if (_interactive && p_dialog->exec() == QDialog::Rejected) return;

  // ATB::Array<float,3>* responseChannelData = new ATB::Array<float,3>(
  //     p_mainWidget->multiChannelModel()->shape(),
  //     p_mainWidget->multiChannelModel()->elementSize());
  // std::memset(responseChannelData->data(), 0,
  //             responseChannelData->size() * sizeof(float));
  // DataChannelSpecs* responseChannel = new DataChannelSpecs(responseChannelData);
  // responseChannel->setColor(blitz::TinyVector<float,3>(1.0, 0.0, 0.0));
  // responseChannel->setGamma(0.25);
  // responseChannel->setUpdatesEnabled(false);

  // ATB::Array<float,3>* radiusChannelData = new ATB::Array<float,3>(
  //     p_mainWidget->multiChannelModel()->shape(),
  //     p_mainWidget->multiChannelModel()->elementSize());
  // std::memset(radiusChannelData->data(), 0,
  //             radiusChannelData->size() * sizeof(float));
  // DataChannelSpecs* radiusChannel = new DataChannelSpecs(radiusChannelData);
  // radiusChannel->setColor(blitz::TinyVector<float,3>(0.0, 1.0, 0.0));
  // radiusChannel->setVisible(false);
  // radiusChannel->setUpdatesEnabled(false);

  AnnotationChannelSpecs* markerChannel =
      new AnnotationChannelSpecs(Marker::Nucleus);
  markerChannel->setName(p_dialog->annotationChannelName());
  markerChannel->setUpdatesEnabled(false);

  p_progress = p_mainWidget->progressReporter();
  p_progress->reset();
  p_progress->setVisible(true);

  // DetectSpheresThread *workerThread = new DetectSpheresThread(
  //     *p_dialog, *markerChannel, p_progress, p_mainWidget, responseChannel,
  //     radiusChannel, p_mainWidget);
  DetectSpheresThread *workerThread = new DetectSpheresThread(
      *p_dialog, *markerChannel, p_progress, p_mainWidget, NULL, NULL,
      p_mainWidget);
  p_mainWidget->setUserInteractionEnabled(false);

  workerThread->start();
  while (workerThread->isRunning()) 
  {
#if defined(_WIN32) || defined(_WIN64)
    Sleep(10);
#else
    usleep(10000);
#endif
    qApp->processEvents();
  }
  if (!p_progress->isAborted())
  {
    // p_mainWidget->multiChannelModel()->addChannel(responseChannel);
    // responseChannel->normalize();
    // responseChannel->setUpdatesEnabled(true);
    // responseChannel->update();
    // p_mainWidget->multiChannelModel()->addChannel(radiusChannel);
    // radiusChannel->normalize();
    // radiusChannel->setUpdatesEnabled(true);
    // radiusChannel->update();
    p_mainWidget->multiChannelModel()->addChannel(markerChannel);
    markerChannel->setUpdatesEnabled(true);
    markerChannel->update();
  }
  delete workerThread;
  p_progress->reset();
  p_progress->setVisible(false);
  p_mainWidget->setUserInteractionEnabled(true);
}

void PluginDetectSpheres::abort()
{
  if (p_progress != NULL) p_progress->abort();
}

Q_EXPORT_PLUGIN2(PluginDetectSpheres, PluginDetectSpheres)
