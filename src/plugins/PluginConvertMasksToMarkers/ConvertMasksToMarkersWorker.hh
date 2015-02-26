#ifndef CONVERTMASKSTOMARKERSWORKER_HH
#define CONVERTMASKSTOMARKERSWORKER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <libProgressReporter/ProgressReporterQt4.hh>

#include <QtCore/QThread>

class ConvertMasksToMarkersParameters;
class LabellingMainWidget;
class AnnotationChannelSpecs;
class VisualizationChannelSpecs;

class ConvertMasksToMarkersWorker : public QThread
{

  Q_OBJECT

  public:
  
  ConvertMasksToMarkersWorker(
      ConvertMasksToMarkersParameters* dialog, LabellingMainWidget* mainWidget,
      AnnotationChannelSpecs* markerChannel,
      iRoCS::ProgressReporterQt4* progress, QWidget* parent = 0);
  ~ConvertMasksToMarkersWorker();

  void run();

  void abort();
  
private:

  ConvertMasksToMarkersParameters* p_dialog;
  LabellingMainWidget* p_mainWidget;
  AnnotationChannelSpecs* p_markerChannel;
  iRoCS::ProgressReporterQt4 *p_progress;

};

#endif
