#ifndef PLUGINATTACHIROCSSCTTOCELLSEGMENTATION_HH
#define PLUGINATTACHIROCSSCTTOCELLSEGMENTATION_HH

#include <liblabelling_qt4/PluginInterface.hh>

#include "AttachIRoCSSCTToCellSegmentationParametersDialog.hh"

#include <QtCore/QtPlugin>

class PluginAttachIRoCSSCTToCellSegmentation :
    public QObject, public PluginInterface
{

  Q_OBJECT
  Q_INTERFACES(PluginInterface)

public:

  PluginAttachIRoCSSCTToCellSegmentation();
  ~PluginAttachIRoCSSCTToCellSegmentation();
  
  QString name() const;
  void setLabellingMainWidget(LabellingMainWidget* parent);
  void setParameters(const std::map<std::string,std::string>& parameters);

  void run();

  void abort();

private:

  LabellingMainWidget* p_mainWidget;
  AttachIRoCSSCTToCellSegmentationParametersDialog* p_dialog;
  bool _interactive;

  iRoCS::ProgressReporterQt4 *p_progress;

};

#endif
