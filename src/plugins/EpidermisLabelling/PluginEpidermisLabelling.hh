#ifndef PLUGINEPIDERMISLABELLING_HH
#define PLUGINEPIDERMISLABELLING_HH

#include <QtCore/QtPlugin>

#include <liblabelling_qt4/PluginInterface.hh>

#include "EpidermisLabellingParametersDialog.hh"

class PluginEpidermisLabelling : public QObject, public PluginInterface
{

  Q_OBJECT
  Q_INTERFACES(PluginInterface)

public:

  PluginEpidermisLabelling();
  ~PluginEpidermisLabelling();
  
  QString name() const;
  void setLabellingMainWidget(LabellingMainWidget* parent);
  void setParameters(const std::map<std::string,std::string>& parameters);

  void run();

  void abort();

private:

  LabellingMainWidget* p_mainWidget;
  EpidermisLabellingParametersDialog* p_dialog;
  bool _interactive;

  iRoCS::ProgressReporterQt4 *p_progress;

};

#endif
