#ifndef PLUGINDETECTNUCLEI_HH
#define PLUGINDETECTNUCLEI_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <liblabelling_qt4/PluginInterface.hh>

#include "DetectNucleiParametersDialog.hh"
#include "DetectNucleiThread.hh"

#include <QtCore/QtPlugin>

class PluginDetectNuclei : public QObject, public PluginInterface
{

  Q_OBJECT
  Q_INTERFACES(PluginInterface)

public:

  PluginDetectNuclei();
  ~PluginDetectNuclei();
  
  QString name() const;
  void setLabellingMainWidget(LabellingMainWidget* parent);
  void setParameters(const std::map<std::string,std::string>& parameters);

  void run();

  void abort();

private:

  LabellingMainWidget* p_mainWidget;
  DetectNucleiParametersDialog* p_dialog;
  DetectNucleiThread* p_workerThread;
  bool _interactive;

  iRoCS::ProgressReporterQt4 *p_progress;

};

#endif
