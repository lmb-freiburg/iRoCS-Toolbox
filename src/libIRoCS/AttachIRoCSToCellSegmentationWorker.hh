#ifndef ATTACHIROCSTOCELLSEGMENTATIONWORKER_HH
#define ATTACHIROCSTOCELLSEGMENTATIONWORKER_HH

#include <QtCore/QThread>

#include <liblabelling_qt4/LabellingMainWidget.hh>

#include <libProgressReporter/ProgressReporter.hh>
#include <libArrayToolbox/iRoCS.hh>

#include "AttachIRoCSToCellSegmentationParameters.hh"

class AttachIRoCSToCellSegmentationWorker : public QThread
{

  Q_OBJECT

  public:
  
  AttachIRoCSToCellSegmentationWorker(
      AttachIRoCSToCellSegmentationParameters const &parameters,
      atb::IRoCS *rct, iRoCS::ProgressReporter *progress = NULL,
      LabellingMainWidget* mainWidget = NULL, QWidget* parent = NULL);
  ~AttachIRoCSToCellSegmentationWorker();

  void run();

private:

  AttachIRoCSToCellSegmentationParameters const &_parameters;
  atb::IRoCS *p_rct;

  iRoCS::ProgressReporter *p_progress;
  LabellingMainWidget *p_mainWidget;

};

#endif
