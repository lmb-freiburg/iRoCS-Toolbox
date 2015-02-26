#ifndef PLUGINDETECTSPHERES_HH
#define PLUGINDETECTSPHERES_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <liblabelling_qt4/PluginInterface.hh>

#include "DetectSpheresParametersDialog.hh"

#include <QtCore/QtPlugin>

class PluginDetectSpheres : public QObject, public PluginInterface
{

  Q_OBJECT
  Q_INTERFACES(PluginInterface)

public:

  PluginDetectSpheres();
  ~PluginDetectSpheres();
  
  QString name() const;
  void setLabellingMainWidget(LabellingMainWidget* parent);
  void setParameters(const std::map<std::string,std::string>& parameters);

  void run();

  void abort();

private:
  
  LabellingMainWidget* p_mainWidget;
  DetectSpheresParametersDialog* p_dialog;
  bool _interactive;

  iRoCS::ProgressReporterQt4 *p_progress;

};

#endif
