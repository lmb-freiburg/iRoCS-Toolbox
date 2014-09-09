#ifndef PLUGINLAYERASSIGNMENT_HH
#define PLUGINLAYERASSIGNMENT_HH

#include <liblabelling_qt4/PluginInterface.hh>

#include "LayerAssignmentParametersDialog.hh"

#include <QtCore/QtPlugin>

class PluginLayerAssignment : public QObject, public PluginInterface
{

  Q_OBJECT
  Q_INTERFACES(PluginInterface)

public:

  PluginLayerAssignment();
  ~PluginLayerAssignment();
  
  QString name() const;
  void setLabellingMainWidget(LabellingMainWidget* parent);
  void setParameters(const std::map<std::string,std::string>& parameters);

  void run();

  void abort();

private:

  LabellingMainWidget* p_mainWidget;
  LayerAssignmentParametersDialog* p_dialog;
  bool _interactive;

  iRoCS::ProgressReporterQt4 *p_progress;

};

#endif
