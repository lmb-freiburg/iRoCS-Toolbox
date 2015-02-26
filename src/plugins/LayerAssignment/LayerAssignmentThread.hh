#ifndef LAYERASSIGNMENTTHREAD_HH
#define LAYERASSIGNMENTTHREAD_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtCore/QThread>

#include <liblabelling_qt4/LabellingMainWidget.hh>

#include <libProgressReporter/ProgressReporter.hh>

#include <libsvmtl/BasicFV.hh>

#include <libIRoCS/LayerAssignmentWorker.hh>

#include "LayerAssignmentParametersDialog.hh"

class LayerAssignmentThread : public QThread
{

  Q_OBJECT

  public:
  
  LayerAssignmentThread(
      LayerAssignmentParametersDialog const &parameters,
      iRoCS::ProgressReporter *progress = NULL,
      LabellingMainWidget* mainWidget = NULL,
      QWidget* parent = NULL);
  ~LayerAssignmentThread();

  void run();

private:

  LayerAssignmentParametersDialog const & _parameters;
  iRoCS::ProgressReporter *p_progress;
  LabellingMainWidget* p_mainWidget;

};

#endif
