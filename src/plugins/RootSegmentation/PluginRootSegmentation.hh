#ifndef PLUGINROOTSEGMENTATION_HH
#define PLUGINROOTSEGMENTATION_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtCore/QtPlugin>

#include <liblabelling_qt4/PluginInterface.hh>

#include "RootSegmentationParametersDialog.hh"
#include "RootSegmentationThread.hh"

#include <libProgressReporter/ProgressReporterQt4.hh>

class PluginRootSegmentation : public QObject, public PluginInterface
{

  Q_OBJECT
  Q_INTERFACES(PluginInterface)

public:

  PluginRootSegmentation();
  ~PluginRootSegmentation();
  
  QString name() const;
  void setLabellingMainWidget(LabellingMainWidget* parent);
  void setParameters(const std::map<std::string,std::string>& parameters);

  void run();

  void abort();

private:

  RootSegmentationThread *p_workerThread;
  LabellingMainWidget* p_mainWidget;
  RootSegmentationParametersDialog* p_dialog;
  bool _interactive;

  iRoCS::ProgressReporterQt4 *p_progress;

};

#endif
