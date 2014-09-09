#ifndef PLUGINASSIGNLAYERSTOCELLSEGMENTATION_HH
#define PLUGINASSIGNLAYERSTOCELLSEGMENTATION_HH

#include <liblabelling_qt4/PluginInterface.hh>

#include "AssignLayersToCellSegmentationParametersDialog.hh"

#include <QtCore/QtPlugin>

class PluginAssignLayersToCellSegmentation :
    public QObject, public PluginInterface
{

  Q_OBJECT
  Q_INTERFACES(PluginInterface)

public:

  PluginAssignLayersToCellSegmentation();
  ~PluginAssignLayersToCellSegmentation();
  
  QString name() const;
  void setLabellingMainWidget(LabellingMainWidget* parent);
  void setParameters(const std::map<std::string,std::string>& parameters);

  void run();

  void abort();

private:

  LabellingMainWidget* p_mainWidget;
  AssignLayersToCellSegmentationParametersDialog* p_dialog;
  bool _interactive;

  iRoCS::ProgressReporterQt4 *p_progress;

};

#endif
