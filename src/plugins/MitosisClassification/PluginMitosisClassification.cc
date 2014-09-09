#include "PluginMitosisClassification.hh"

#include <QtGui/QMessageBox>
#include <QtGui/QApplication>

#include <liblabelling_qt4/MultiChannelModel.hh>
#include <liblabelling_qt4/LabellingMainWidget.hh>
#include <liblabelling_qt4/ChannelSelectionControlElement.hh>
#include <liblabelling_qt4/FileNameSelectionControlElement.hh>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <unistd.h>
#endif

PluginMitosisClassification::PluginMitosisClassification() 
        : QObject(), p_mainWidget(NULL), p_dialog(NULL), p_workerThread(NULL),
          _interactive(true), p_progress(NULL)
{}

PluginMitosisClassification::~PluginMitosisClassification()
{
  delete p_dialog;
}

QString PluginMitosisClassification::name() const
{
  return QString(tr("Classify Mitoses"));
}

void PluginMitosisClassification::setLabellingMainWidget(
    LabellingMainWidget* parent)
{
  p_mainWidget = parent;
  delete p_dialog;
  p_dialog = new MitosisClassificationParametersDialog(
      p_mainWidget, p_mainWidget);
}

void PluginMitosisClassification::setParameters(
    const std::map<std::string,std::string>& parameters)
{
  if (parameters.find("dataChannel") != parameters.end())
      p_dialog->setDataChannel(
          static_cast<DataChannelSpecs*>(
              p_mainWidget->multiChannelModel()->channel(
                  parameters.find("dataChannel")->second)));

  if (parameters.find("annotationChannel") != parameters.end())
      p_dialog->setAnnotationChannel(
          static_cast<AnnotationChannelSpecs*>(
              p_mainWidget->multiChannelModel()->channel(
                  parameters.find("annotationChannel")->second)));

  if (parameters.find("cacheFile") != parameters.end())
      p_dialog->setCacheFileName(parameters.find("cacheFile")->second);

  if (parameters.find("modelFile") != parameters.end())
      p_dialog->setModelFileName(parameters.find("modelFile")->second);

  _interactive = false;
}

void PluginMitosisClassification::run()
{
  // Check whether at least one annotation channel is loaded, otherwise give up
  if (p_mainWidget->multiChannelModel()->nChannels(ChannelSpecs::Annotation)
      == 0)
  {
    QMessageBox::warning(
        p_mainWidget, tr("Plugin prerequisites not met"),
        tr("This plugin needs one annotation channel to classify nucleus "
           "candidates in. Please load an annotation channel."));
    return;
  }
  if (_interactive && p_dialog->exec() == QDialog::Rejected) return;

  p_progress = p_mainWidget->progressReporter();
  p_progress->reset();
  p_progress->setVisible(true);

  p_workerThread = new MitosisClassificationWorker(
      *p_dialog, p_progress, p_mainWidget, p_mainWidget);
  p_mainWidget->setUserInteractionEnabled(false);
  p_dialog->annotationChannel()->setUpdatesEnabled(false);

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
  delete p_workerThread;
  p_workerThread = NULL;
  p_progress->reset();
  p_progress->setVisible(false);
  p_mainWidget->setUserInteractionEnabled(true);
  p_dialog->annotationChannel()->setUpdatesEnabled(true);
  p_dialog->annotationChannel()->update();
}

void PluginMitosisClassification::abort()
{
  if (p_progress != NULL) p_progress->abort();
}

Q_EXPORT_PLUGIN2(PluginMitosisClassification, PluginMitosisClassification)
