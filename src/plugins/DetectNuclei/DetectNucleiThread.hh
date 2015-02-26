#ifndef DETECTNUCLEITHREAD_HH
#define DETECTNUCLEITHREAD_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtCore/QThread>

#include <liblabelling_qt4/LabellingMainWidget.hh>

#include <libProgressReporter/ProgressReporter.hh>

#include "DetectNucleiParametersDialog.hh"

class DetectNucleiThread : public QThread
{

  Q_OBJECT

  public:
  
  DetectNucleiThread(
      DetectNucleiParametersDialog const &parameters,
      AnnotationChannelSpecs &markerChannel,
      iRoCS::ProgressReporter *progress = NULL,
      LabellingMainWidget *mainWidget = NULL,
      QWidget *parent = NULL);
  ~DetectNucleiThread();

  void run();

private:

  DetectNucleiParametersDialog const &_parameters;
  AnnotationChannelSpecs &_markerChannel;
  iRoCS::ProgressReporter *p_progress;
  LabellingMainWidget* p_mainWidget;

};

#endif
