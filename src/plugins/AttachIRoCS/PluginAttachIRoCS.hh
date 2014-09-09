#ifndef PLUGINATTACHIROCS_HH
#define PLUGINATTACHIROCS_HH

#include <liblabelling_qt4/PluginInterface.hh>

#include "AttachIRoCSParametersDialog.hh"

#include <QtCore/QtPlugin>

class PluginAttachIRoCS : public QObject, public PluginInterface
{

  Q_OBJECT
  Q_INTERFACES(PluginInterface)

public:

  PluginAttachIRoCS();
  ~PluginAttachIRoCS();
  
  QString name() const;
  void setLabellingMainWidget(LabellingMainWidget* parent);
  void setParameters(const std::map<std::string,std::string>& parameters);

  void run();

  void abort();

private:

  LabellingMainWidget* p_mainWidget;
  AttachIRoCSParametersDialog* p_dialog;
  bool _interactive;

  iRoCS::ProgressReporterQt4 *p_progress;

};

#endif
