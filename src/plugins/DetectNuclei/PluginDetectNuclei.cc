#include "PluginDetectNuclei.hh"

#include <QtGui/QMessageBox>
#include <QtGui/QApplication>

#include <liblabelling_qt4/MultiChannelModel.hh>
#include <liblabelling_qt4/LabellingMainWidget.hh>
#include <liblabelling_qt4/AnnotationChannelSpecs.hh>
#include <liblabelling_qt4/ChannelSelectionControlElement.hh>
#include <liblabelling_qt4/FileNameSelectionControlElement.hh>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <unistd.h>
#endif

PluginDetectNuclei::PluginDetectNuclei() 
        : QObject(), p_mainWidget(NULL), p_dialog(NULL), p_workerThread(NULL),
          _interactive(true), p_progress(NULL)
{}

PluginDetectNuclei::~PluginDetectNuclei()
{
  delete p_dialog;
}

QString PluginDetectNuclei::name() const
{
  return QString(tr("01 - Detect Nuclei"));
}

void PluginDetectNuclei::setLabellingMainWidget(LabellingMainWidget* parent)
{
  p_mainWidget = parent;
  delete p_dialog;
  p_dialog = new DetectNucleiParametersDialog(p_mainWidget, p_mainWidget);
}

void PluginDetectNuclei::setParameters(
    const std::map<std::string,std::string>& parameters)
{
  if (parameters.find("dataChannel") != parameters.end())
      p_dialog->setDataChannel(
          p_mainWidget->multiChannelModel()->channel(
              parameters.find("dataChannel")->second));

  if (parameters.find("annotationChannel") != parameters.end())
      p_dialog->setAnnotationChannelName(
          parameters.find("annotationChannel")->second);

  if (parameters.find("cacheFile") != parameters.end())
      p_dialog->setCacheFileName(parameters.find("cacheFile")->second);

  if (parameters.find("modelFile") != parameters.end())
      p_dialog->setModelFileName(parameters.find("modelFile")->second);

  _interactive = false;
}

void PluginDetectNuclei::run()
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
           "nucleus candidates in. Please load a data channel."));
    return;
  }
  if (_interactive && p_dialog->exec() == QDialog::Rejected) return;

  if (p_mainWidget != NULL)
      p_dialog->setMemoryLimit(p_mainWidget->memoryLimit());

  AnnotationChannelSpecs* markerChannel =
      new AnnotationChannelSpecs(Marker::Nucleus);
  markerChannel->setName(p_dialog->annotationChannelName());

  p_progress = p_mainWidget->progressReporter();
  p_progress->reset();
  p_progress->setVisible(true);

  p_workerThread = new DetectNucleiThread(
      *p_dialog, *markerChannel, p_progress, p_mainWidget, p_mainWidget);
  p_mainWidget->setUserInteractionEnabled(false);

  p_workerThread->start();
  while (p_workerThread->isRunning()) 
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
    p_mainWidget->multiChannelModel()->addChannel(markerChannel);
    markerChannel->setVisible(true);
    markerChannel->setUpdatesEnabled(true);
    markerChannel->update();
  }
  else delete markerChannel;

  delete p_workerThread;
  p_workerThread = NULL;
  p_progress->reset();
  p_progress->setVisible(false);
  p_mainWidget->setUserInteractionEnabled(true);
}

void PluginDetectNuclei::abort()
{
  if (p_progress != NULL) p_progress->abort();
}

Q_EXPORT_PLUGIN2(PluginDetectNuclei, PluginDetectNuclei)
