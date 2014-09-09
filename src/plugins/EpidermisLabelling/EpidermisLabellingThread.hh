#ifndef EPIDERMISLABELLINGTHREAD_HH
#define EPIDERMISLABELLINGTHREAD_HH

#include <QtCore/QThread>

#include <liblabelling_qt4/LabellingMainWidget.hh>

#include <libProgressReporter/ProgressReporter.hh>

#include <libsvmtl/BasicFV.hh>

#include "EpidermisLabellingParametersDialog.hh"

class EpidermisLabellingThread : public QThread
{

  Q_OBJECT

  public:
  
  EpidermisLabellingThread(
      EpidermisLabellingParametersDialog const &parameters,
      iRoCS::ProgressReporter *progress = NULL,
      LabellingMainWidget* mainWidget = NULL,
      QWidget* parent = NULL);
  ~EpidermisLabellingThread();

  void run();

private:

  EpidermisLabellingParametersDialog const &_parameters;
  iRoCS::ProgressReporter *p_progress;
  LabellingMainWidget* p_mainWidget;

};

#endif
