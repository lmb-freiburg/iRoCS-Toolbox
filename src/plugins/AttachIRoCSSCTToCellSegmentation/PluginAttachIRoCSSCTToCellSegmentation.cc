#include "PluginAttachIRoCSSCTToCellSegmentation.hh"
#include "AttachIRoCSSCTToCellSegmentationThread.hh"

#include <QtGui/QMessageBox>
#include <QtGui/QApplication>

#include <liblabelling_qt4/MultiChannelModel.hh>
#include <liblabelling_qt4/LabellingMainWidget.hh>
#include <liblabelling_qt4/VisualizationChannelSpecs.hh>
#include <liblabelling_qt4/IRoCSSCTChannelSpecs.hh>
#include <liblabelling_qt4/ChannelSelectionControlElement.hh>
#include <liblabelling_qt4/FileNameSelectionControlElement.hh>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <unistd.h>
#endif

PluginAttachIRoCSSCTToCellSegmentation::
PluginAttachIRoCSSCTToCellSegmentation() 
        : QObject(), p_mainWidget(NULL), p_dialog(NULL), _interactive(true),
          p_progress(NULL)
{}

PluginAttachIRoCSSCTToCellSegmentation::
~PluginAttachIRoCSSCTToCellSegmentation()
{
  delete p_dialog;
}

QString PluginAttachIRoCSSCTToCellSegmentation::name() const
{
  return tr("C02 - Attach iRoCS to segmented cells");
}

void PluginAttachIRoCSSCTToCellSegmentation::setLabellingMainWidget(
    LabellingMainWidget* parent)
{
  p_mainWidget = parent;
  delete p_dialog;
  p_dialog = new AttachIRoCSSCTToCellSegmentationParametersDialog(
      p_mainWidget, p_mainWidget);
}

void PluginAttachIRoCSSCTToCellSegmentation::setParameters(
    const std::map<std::string,std::string>& parameters)
{
  std::map<std::string,std::string>::const_iterator it;
  if ((it = parameters.find("segmentationChannel")) != parameters.end())
      p_dialog->setSegmentationChannel(
          static_cast<VisualizationChannelSpecs*>(
              p_mainWidget->multiChannelModel()->channel(it->second)));
  if ((it = parameters.find("qcChannel")) != parameters.end())
      p_dialog->setQcChannel(
          static_cast<AnnotationChannelSpecs*>(
              p_mainWidget->multiChannelModel()->channel(it->second)));
  if ((it = parameters.find("iRoCSChannelName")) != parameters.end())
      p_dialog->setIRoCSChannelName(it->second);
  if ((it = parameters.find("backgroundLabel")) != parameters.end())
      p_dialog->setBackgroundLabel(atoi(it->second.c_str()));
  if ((it = parameters.find("segmentLengthUm")) != parameters.end())
      p_dialog->setSegmentLengthUm(atof(it->second.c_str()));
  if ((it = parameters.find("marginUm")) != parameters.end())
      p_dialog->setMarginUm(atof(it->second.c_str()));
  if ((it = parameters.find("downsampleRatio")) != parameters.end())
      p_dialog->setDownsampleRatio(atoi(it->second.c_str()));
  _interactive = false;
}

void PluginAttachIRoCSSCTToCellSegmentation::run()
{
  // Check whether at least one visualization and one annotation channel are
  // loaded, otherwise give up
  if (p_mainWidget->multiChannelModel()->nChannels(ChannelSpecs::Annotation)
      < 1 ||
      p_mainWidget->multiChannelModel()->nChannels(ChannelSpecs::Visualization)
      < 1)
  {
    QMessageBox::warning(
        p_mainWidget, tr("Plugin prerequisites not met"),
        tr("This plugin needs one visualization channel containing "
           "a cellular segmentation and one annotation channel "
           "containing one marker at the QC position.\nThe segmentation "
           "channel typically is the output of the 'Segment Cells' "
           "plugin.\nThe QC annotation channel has to be prepared manually "
           "by creating a new annotation channel and clicking the QC "
           "position once. Only the first marker of the channel will be "
           "regarded, so at best specify only one to not confuse yourself."));
    return;
  }
  if (_interactive && p_dialog->exec() == QDialog::Rejected) return;

  p_progress = p_mainWidget->progressReporter();
  p_progress->reset();
  p_progress->setVisible(true);

  ShellCoordinateTransform *sct = new ShellCoordinateTransform();

  AttachIRoCSSCTToCellSegmentationThread *workerThread =
      new AttachIRoCSSCTToCellSegmentationThread(
          *p_dialog, sct, p_progress, p_mainWidget, p_mainWidget);
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
    IRoCSSCTChannelSpecs* channel =
        p_mainWidget->multiChannelModel()->addIRoCSSCTChannel(sct);
    if (channel != NULL)
    {
      channel->setName(p_dialog->iRoCSChannelName());
      channel->setColor(blitz::TinyVector<float,3>(1.0, 1.0, 0.0));
    }
    else
    {
      QMessageBox::warning(
          p_mainWidget, tr("Channel creation failed"),
          tr("Root axis channel could not be created. The root coordinate "
             "system is invalid."));
    }
  }
  delete workerThread;
  p_progress->reset();
  p_progress->setVisible(false);
  p_mainWidget->setUserInteractionEnabled(true);
}

void PluginAttachIRoCSSCTToCellSegmentation::abort()
{
  if (p_progress != NULL) p_progress->abort();
}

Q_EXPORT_PLUGIN2(PluginAttachIRoCSSCTToCellSegmentation, PluginAttachIRoCSSCTToCellSegmentation)
