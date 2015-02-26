#ifndef PLUGINMITOSISCLASSIFICATION_HH
#define PLUGINMITOSISCLASSIFICATION_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <liblabelling_qt4/PluginInterface.hh>

#include "MitosisClassificationParametersDialog.hh"

#include <libIRoCS/MitosisClassificationWorker.hh>

#include <QtCore/QtPlugin>

class PluginMitosisClassification : public QObject, public PluginInterface
{

  Q_OBJECT
  Q_INTERFACES(PluginInterface)

public:

  PluginMitosisClassification();
  ~PluginMitosisClassification();
  
  QString name() const;
  void setLabellingMainWidget(LabellingMainWidget* parent);
  void setParameters(const std::map<std::string,std::string>& parameters);

  void run();

  void abort();

private:

  LabellingMainWidget* p_mainWidget;
  MitosisClassificationParametersDialog* p_dialog;
  MitosisClassificationWorker* p_workerThread;
  bool _interactive;

  iRoCS::ProgressReporterQt4 *p_progress;

};

#endif
