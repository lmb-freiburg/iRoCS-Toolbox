#include "PluginAttachIRoCSToCellSegmentation.hh"

#include <QtGui/QMessageBox>
#include <QtGui/QApplication>

#include <liblabelling_qt4/MultiChannelModel.hh>
#include <liblabelling_qt4/LabellingMainWidget.hh>
#include <liblabelling_qt4/IRoCSChannelSpecs.hh>
#include <liblabelling_qt4/VisualizationChannelSpecs.hh>
#include <liblabelling_qt4/ChannelSelectionControlElement.hh>
#include <liblabelling_qt4/FileNameSelectionControlElement.hh>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <unistd.h>
#endif

PluginAttachIRoCSToCellSegmentation::PluginAttachIRoCSToCellSegmentation() 
        : QObject(), p_mainWidget(NULL), p_dialog(NULL), p_workerThread(NULL),
          _interactive(true), p_progress(NULL)
{}

PluginAttachIRoCSToCellSegmentation::~PluginAttachIRoCSToCellSegmentation()
{
  delete p_dialog;
}

QString PluginAttachIRoCSToCellSegmentation::name() const
{
  return tr("C02 - Attach iRoCS to segmented cells");
}

void PluginAttachIRoCSToCellSegmentation::setLabellingMainWidget(
    LabellingMainWidget* parent)
{
  p_mainWidget = parent;
  delete p_dialog;
  p_dialog = new AttachIRoCSToCellSegmentationParametersDialog(
      p_mainWidget, p_mainWidget);
}

void PluginAttachIRoCSToCellSegmentation::setParameters(
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
  if ((it = parameters.find("nIterations")) != parameters.end())
      p_dialog->setNIterations(atoi(it->second.c_str()));
  if ((it = parameters.find("dataWeight")) != parameters.end())
      p_dialog->setDataWeight(atof(it->second.c_str()));
  if ((it = parameters.find("axisStiffnessWeight")) != parameters.end())
      p_dialog->setAxisStiffnessWeight(atof(it->second.c_str()));
  if ((it = parameters.find("thicknessConstancyWeight")) != parameters.end())
      p_dialog->setThicknessConstancyWeight(atof(it->second.c_str()));
  if ((it = parameters.find("optimizationTimeStep")) != parameters.end())
      p_dialog->setOptimizationTimeStep(atof(it->second.c_str()));
  _interactive = false;
}

void PluginAttachIRoCSToCellSegmentation::run()
{
  // Check whether at least one visualization and one annotation channel are
  // loaded, otherwise give up
  if (p_mainWidget->multiChannelModel()->nChannels(ChannelSpecs::Annotation)
      == 0 ||
      p_mainWidget->multiChannelModel()->nChannels(ChannelSpecs::Visualization)
      == 0)
  {
    QMessageBox::warning(
        p_mainWidget, tr("Plugin prerequisites not met"),
        tr("This plugin needs one visualization channel containing "
           "the cell segmentation and one annotation channel "
           "containing one marker at the QC position.\nThe segmentation is "
           "obtained by running the 'C01 - Segment root' plugin.\n"
           "The QC annotation channel has to be prepared manually by "
           "creating a new annotation channel and clicking the QC position "
           "once. Only the first marker of the channel will be regarded, so "
           "at best specify only one to not confuse yourself."));
    return;
  }
  if (_interactive && p_dialog->exec() == QDialog::Rejected) return;

  p_progress = p_mainWidget->progressReporter();
  p_progress->reset();
  p_progress->setVisible(true);

  atb::IRoCS *rct = new atb::IRoCS(p_progress);

  p_workerThread = new AttachIRoCSToCellSegmentationWorker(
      *p_dialog, rct, p_progress, p_mainWidget, p_mainWidget);
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
    IRoCSChannelSpecs* channel =
        p_mainWidget->multiChannelModel()->addIRoCSChannel(rct);
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
  delete p_workerThread;
  p_workerThread = NULL;
  p_progress->reset();
  p_progress->setVisible(false);
  p_mainWidget->setUserInteractionEnabled(true);
}

void PluginAttachIRoCSToCellSegmentation::abort()
{
  if (p_progress != NULL) p_progress->abort();
}

Q_EXPORT_PLUGIN2(PluginAttachIRoCSToCellSegmentation,
                 PluginAttachIRoCSToCellSegmentation)
