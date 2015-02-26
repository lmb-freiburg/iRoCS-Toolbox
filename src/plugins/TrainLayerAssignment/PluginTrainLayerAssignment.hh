#ifndef PLUGINTRAINLAYERASSIGNMENT_HH
#define PLUGINTRAINLAYERASSIGNMENT_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <liblabelling_qt4/PluginInterface.hh>

#include "TrainLayerAssignmentParametersDialog.hh"

#include <libIRoCS/TrainLayerAssignmentWorker.hh>

#include <QtCore/QtPlugin>

class PluginTrainLayerAssignment : public QObject, public PluginInterface
{

  Q_OBJECT
  Q_INTERFACES(PluginInterface)

public:

  PluginTrainLayerAssignment();
  ~PluginTrainLayerAssignment();
  
  QString name() const;
  void setLabellingMainWidget(LabellingMainWidget* parent);
  void setParameters(const std::map<std::string,std::string>& parameters);

  void run();

  void abort();

private:

  LabellingMainWidget* p_mainWidget;
  TrainLayerAssignmentParametersDialog* p_dialog;
  TrainLayerAssignmentWorker* p_workerThread;
  bool _interactive;

  iRoCS::ProgressReporterQt4 *p_progress;

};

#endif
