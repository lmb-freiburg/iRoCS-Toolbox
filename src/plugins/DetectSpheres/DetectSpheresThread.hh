#ifndef DETECTSPHERESTHREAD_HH
#define DETECTSPHERESTHREAD_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtCore/QThread>

#include <liblabelling_qt4/LabellingMainWidget.hh>
#include <liblabelling_qt4/DataChannelSpecs.hh>
#include <liblabelling_qt4/AnnotationChannelSpecs.hh>

#include <libProgressReporter/ProgressReporter.hh>

#include "DetectSpheresParametersDialog.hh"

class DetectSpheresThread : public QThread
{

  Q_OBJECT

  public:
  
  DetectSpheresThread(
      DetectSpheresParametersDialog const &parameters,
      AnnotationChannelSpecs &markerChannel,
      iRoCS::ProgressReporter *progress = NULL,
      LabellingMainWidget *mainWidget = NULL,
      DataChannelSpecs *responseChannel = NULL,
      DataChannelSpecs *radiusUmChannel = NULL,
      QWidget *parent = NULL);
  ~DetectSpheresThread();

  void run();

private:

  DetectSpheresParametersDialog const &_parameters;
  AnnotationChannelSpecs &_markerChannel;
  iRoCS::ProgressReporter *p_progress;
  LabellingMainWidget *p_mainWidget;
  DataChannelSpecs *p_responseChannel, *p_radiusUmChannel;

};

#endif
