#ifndef ASSIGNLAYERSTOCELLSEGMENTATIONTHREAD_HH
#define ASSIGNLAYERSTOCELLSEGMENTATIONTHREAD_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtCore/QThread>

#include <liblabelling_qt4/LabellingMainWidget.hh>

#include <libProgressReporter/ProgressReporter.hh>

#include "AssignLayersToCellSegmentationParametersDialog.hh"

class AssignLayersToCellSegmentationThread : public QThread
{

  Q_OBJECT

  public:
  
  AssignLayersToCellSegmentationThread(
      AssignLayersToCellSegmentationParametersDialog const &parameters,
      iRoCS::ProgressReporter *progress = NULL,
      LabellingMainWidget* mainWidget = NULL, QWidget* parent = NULL);
  ~AssignLayersToCellSegmentationThread();

  void run();

private:

  AssignLayersToCellSegmentationParametersDialog const &_parameters;
  iRoCS::ProgressReporter *p_progress;
  LabellingMainWidget *p_mainWidget;

};

#endif
