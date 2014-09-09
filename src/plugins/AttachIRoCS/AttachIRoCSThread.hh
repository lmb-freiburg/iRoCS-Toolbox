#ifndef ATTACHIROCSTHREAD_HH
#define ATTACHIROCSTHREAD_HH

#include <QtCore/QThread>

#include <liblabelling_qt4/LabellingMainWidget.hh>

#include <libProgressReporter/ProgressReporter.hh>

#include <libArrayToolbox/iRoCS.hh>

#include "AttachIRoCSParametersDialog.hh"

class AttachIRoCSThread : public QThread
{

  Q_OBJECT

  public:
  
  AttachIRoCSThread(
      AttachIRoCSParametersDialog const &parameters, atb::IRoCS *rct,
      iRoCS::ProgressReporter *progress = NULL,
      LabellingMainWidget* mainWidget = NULL, QWidget* parent = NULL);
  ~AttachIRoCSThread();

  void run();

private:

  AttachIRoCSParametersDialog const &_parameters;
  atb::IRoCS *p_rct;
  iRoCS::ProgressReporter *p_progress;
  LabellingMainWidget *p_mainWidget;

};

#endif
