#ifndef ATTACHIROCSSCTTOCELLSEGMENTATIONTHREAD_HH
#define ATTACHIROCSSCTTOCELLSEGMENTATIONTHREAD_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtCore/QThread>

#include <liblabelling_qt4/LabellingMainWidget.hh>

#include <libProgressReporter/ProgressReporter.hh>

#include <libArrayToolbox/algo/ShellCoordinateTransform.hh>

#include "AttachIRoCSSCTToCellSegmentationParametersDialog.hh"

class AttachIRoCSSCTToCellSegmentationThread : public QThread
{

  Q_OBJECT

  public:
  
  AttachIRoCSSCTToCellSegmentationThread(
      AttachIRoCSSCTToCellSegmentationParametersDialog const &parameters,
      ShellCoordinateTransform *sct, iRoCS::ProgressReporter *progress = NULL,
      LabellingMainWidget* mainWidget = NULL, QWidget* parent = NULL);
  ~AttachIRoCSSCTToCellSegmentationThread();

  void run();

private:

  AttachIRoCSSCTToCellSegmentationParametersDialog const &_parameters;
  ShellCoordinateTransform *p_sct;
  iRoCS::ProgressReporter *p_progress;
  LabellingMainWidget *p_mainWidget;

};

#endif
