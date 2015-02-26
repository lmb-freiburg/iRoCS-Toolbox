#ifndef PLUGINTRAINEPIDERMISLABELLING_HH
#define PLUGINTRAINEPIDERMISLABELLING_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <liblabelling_qt4/PluginInterface.hh>

#include "TrainEpidermisLabellingParametersDialog.hh"

#include <libIRoCS/TrainEpidermisLabellingWorker.hh>

#include <QtCore/QtPlugin>

class PluginTrainEpidermisLabelling : public QObject, public PluginInterface
{

  Q_OBJECT
  Q_INTERFACES(PluginInterface)

public:

  PluginTrainEpidermisLabelling();
  ~PluginTrainEpidermisLabelling();
  
  QString name() const;
  void setLabellingMainWidget(LabellingMainWidget* parent);
  void setParameters(const std::map<std::string,std::string>& parameters);

  void run();

  void abort();

private:

  LabellingMainWidget* p_mainWidget;
  TrainEpidermisLabellingParametersDialog* p_dialog;
  TrainEpidermisLabellingWorker* p_workerThread;
  bool _interactive;

  iRoCS::ProgressReporterQt4 *p_progress;

};

#endif
