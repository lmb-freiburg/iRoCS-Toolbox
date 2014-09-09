#include "PluginTrainEpidermisLabelling.hh"

#include <QtGui/QMessageBox>
#include <QtGui/QStatusBar>
#include <QtGui/QProgressBar>
#include <QtGui/QApplication>

#include <liblabelling_qt4/MultiChannelModel.hh>
#include <liblabelling_qt4/LabellingMainWidget.hh>
#include <liblabelling_qt4/FileNameSelectionControlElement.hh>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <unistd.h>
#endif

PluginTrainEpidermisLabelling::PluginTrainEpidermisLabelling() 
        : QObject(), p_mainWidget(NULL), p_dialog(NULL), p_workerThread(NULL),
          _interactive(true), p_progress(NULL)
{}

PluginTrainEpidermisLabelling::~PluginTrainEpidermisLabelling()
{
  delete p_dialog;
}

QString PluginTrainEpidermisLabelling::name() const
{
  return QString("[iRoCS] Train Epidermis Labelling");
}

void PluginTrainEpidermisLabelling::setLabellingMainWidget(
    LabellingMainWidget* parent)
{
  p_mainWidget = parent;
  delete p_dialog;
  p_dialog = new TrainEpidermisLabellingParametersDialog(
      p_mainWidget, p_mainWidget);
}

void PluginTrainEpidermisLabelling::setParameters(
    const std::map<std::string,std::string>&)
{
  // Within labelling this can be only used interactively!
}

void PluginTrainEpidermisLabelling::run()
{
  if (_interactive && p_dialog->exec() == QDialog::Rejected) return;

  p_progress = p_mainWidget->progressReporter();
  p_progress->reset();
  p_progress->setVisible(true);

  p_workerThread = new TrainEpidermisLabellingWorker(*p_dialog, p_progress);
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
  delete p_workerThread;
  p_workerThread = NULL;
  p_progress->reset();
  p_progress->setVisible(false);
  p_mainWidget->setUserInteractionEnabled(true);
}

void PluginTrainEpidermisLabelling::abort()
{
  if (p_progress != NULL) p_progress->abort();
}

Q_EXPORT_PLUGIN2(PluginTrainEpidermisLabelling, PluginTrainEpidermisLabelling)
