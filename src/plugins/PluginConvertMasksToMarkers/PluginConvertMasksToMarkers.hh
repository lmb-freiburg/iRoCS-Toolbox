#ifndef PLUGINCONVERTMASKSTOMARKERS_HH
#define PLUGINCONVERTMASKSTOMARKERS_HH

#include <liblabelling_qt4/PluginInterface.hh>

#include "ConvertMasksToMarkersParameters.hh"
#include "ConvertMasksToMarkersWorker.hh"

#include <QtCore/QtPlugin>

class PluginConvertMasksToMarkers : public QObject, public PluginInterface
{

  Q_OBJECT
  Q_INTERFACES(PluginInterface)

public:

  PluginConvertMasksToMarkers();
  ~PluginConvertMasksToMarkers();
  
  QString name() const;
  void setLabellingMainWidget(LabellingMainWidget* parent);
  void setParameters(const std::map<std::string,std::string>& parameters);

  void run();

  void abort();

private:

  LabellingMainWidget* p_mainWidget;
  ConvertMasksToMarkersParameters* p_dialog;
  ConvertMasksToMarkersWorker* p_workerThread;
  bool _interactive;

};

#endif
