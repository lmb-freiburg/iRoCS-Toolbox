#include "PluginRootSegmentation.hh"

#include <QtGui/QMessageBox>
#include <QtGui/QStatusBar>
#include <QtGui/QProgressBar>
#include <QtGui/QApplication>

#include <liblabelling_qt4/LabellingMainWidget.hh>
#include <liblabelling_qt4/MultiChannelModel.hh>
#include <liblabelling_qt4/DataChannelSpecs.hh>
#include <liblabelling_qt4/VisualizationChannelSpecs.hh>
#include <liblabelling_qt4/ColorMap.hh>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <unistd.h>
#endif

PluginRootSegmentation::PluginRootSegmentation() 
        : QObject(), p_workerThread(NULL), p_mainWidget(NULL),
          p_dialog(NULL), _interactive(true), p_progress(NULL)
{}

PluginRootSegmentation::~PluginRootSegmentation()
{
  delete p_dialog;
}

QString PluginRootSegmentation::name() const
{
  return tr("C01 - Segment Root");
}

void PluginRootSegmentation::setLabellingMainWidget(LabellingMainWidget* parent)
{
  p_mainWidget = parent;
  delete p_dialog;
  p_dialog = new RootSegmentationParametersDialog(p_mainWidget, p_mainWidget);
}

void PluginRootSegmentation::setParameters(
    const std::map<std::string,std::string>& parameters)
{
  if (parameters.find("dataChannel") != parameters.end())
      p_dialog->setDataChannel(
          p_mainWidget->multiChannelModel()->channel(
                  parameters.find("dataChannel")->second));

  std::cerr << __FILE__ << ":" << __LINE__ << ": Parameter map parsing is not "
            << "yet implemented! Using default parameters" << std::endl;

  _interactive = false;
}

void PluginRootSegmentation::run()
{
  if (p_mainWidget == NULL)
  {
    std::cerr << "Assertion failed: LabellingMainWidget* p_mainWidget == "
              << "NULL in " << __FILE__ << ":" << __LINE__
              << ". Did you call setLabellingMainWidget() before running the "
              << "plugin?" << std::endl;
    return;
  }
  if (p_dialog == NULL)
  {
    std::cerr << "Assertion failed: RootSegmentationParameters* p_dialog == "
              << "NULL in " << __FILE__ << ":" << __LINE__
              << ". Did you call setLabellingMainWidget() before running the "
              << "plugin?" << std::endl;
    return;
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
        tr("This plugin needs one data or visualization channel to segment. "
           "Please load a data channel."));
    return;
  }
  if (_interactive && p_dialog->exec() == QDialog::Rejected) return;

  atb::Array<int,3>* segmentationData = new atb::Array<int,3>();
  VisualizationChannelSpecs *segmentationChannel =
      new VisualizationChannelSpecs(segmentationData);
  segmentationChannel->setName(p_dialog->segmentationChannelName());
  segmentationChannel->setUpdatesEnabled(false);

  p_progress = p_mainWidget->progressReporter();
  p_progress->reset();
  p_progress->setVisible(true);

  p_workerThread = new RootSegmentationThread(
      *p_dialog, *segmentationChannel, p_progress, p_mainWidget);
  connect(p_workerThread, SIGNAL(error(QString const &)),
          p_mainWidget, SLOT(showError(QString const &)));
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

  if (p_progress->isAborted()) delete segmentationChannel;
  else
  {
    segmentationChannel->revalidate();
    p_mainWidget->multiChannelModel()->addChannel(segmentationChannel);
    segmentationChannel->colorMap().setRandom();
    segmentationChannel->colorMap().addOverrideColor(
        0, blitz::TinyVector<float,3>(0.0f));
    segmentationChannel->colorMap().addOverrideColor(
        1, blitz::TinyVector<float,3>(0.0f));
    segmentationChannel->normalizeIndexRange();
    segmentationChannel->setUpdatesEnabled(true);
    segmentationChannel->update();
  }
  delete p_workerThread;
  p_workerThread = NULL;
  p_progress->reset();
  p_progress->setVisible(false);
  p_mainWidget->setUserInteractionEnabled(true);
}

void PluginRootSegmentation::abort()
{
  if (p_workerThread != NULL) p_progress->abort();  
}

Q_EXPORT_PLUGIN2(PluginRootSegmentation, PluginRootSegmentation)
