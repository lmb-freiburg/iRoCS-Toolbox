#ifndef PLUGINATTACHIROCSTOCELLSEGMENTATION_HH
#define PLUGINATTACHIROCSTOCELLSEGMENTATION_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <liblabelling_qt4/PluginInterface.hh>

#include "AttachIRoCSToCellSegmentationParametersDialog.hh"

#include <libIRoCS/AttachIRoCSToCellSegmentationWorker.hh>

#include <QtCore/QtPlugin>

class PluginAttachIRoCSToCellSegmentation
    : public QObject, public PluginInterface
{

  Q_OBJECT
  Q_INTERFACES(PluginInterface)

public:

  PluginAttachIRoCSToCellSegmentation();
  ~PluginAttachIRoCSToCellSegmentation();
  
  QString name() const;
  void setLabellingMainWidget(LabellingMainWidget* parent);
  void setParameters(const std::map<std::string,std::string>& parameters);

  void run();

  void abort();

private:

  LabellingMainWidget* p_mainWidget;
  AttachIRoCSToCellSegmentationParametersDialog* p_dialog;
  AttachIRoCSToCellSegmentationWorker* p_workerThread;
  bool _interactive;

  iRoCS::ProgressReporterQt4 *p_progress;

};

#endif
