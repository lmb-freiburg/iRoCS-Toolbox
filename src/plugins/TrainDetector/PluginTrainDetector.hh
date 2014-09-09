#ifndef PLUGINTRAINDETECTOR_HH
#define PLUGINTRAINDETECTOR_HH

#include <liblabelling_qt4/PluginInterface.hh>

#include "TrainDetectorParametersDialog.hh"

#include <libIRoCS/TrainDetectorWorker.hh>

#include <QtCore/QtPlugin>

class PluginTrainDetector : public QObject, public PluginInterface
{

  Q_OBJECT
  Q_INTERFACES(PluginInterface)

public:

  PluginTrainDetector();
  ~PluginTrainDetector();
  
  QString name() const;
  void setLabellingMainWidget(LabellingMainWidget* parent);
  void setParameters(const std::map<std::string,std::string>& parameters);

  void run();

  void abort();

private:

  LabellingMainWidget* p_mainWidget;
  TrainDetectorParametersDialog* p_dialog;
  TrainDetectorWorker* p_workerThread;
  bool _interactive;

  iRoCS::ProgressReporterQt4 *p_progress;

};

#endif
