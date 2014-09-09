#ifndef TRAINEPIDERMISLABELLINGWORKER_HH
#define TRAINEPIDERMISLABELLINGWORKER_HH

#include <QtCore/QThread>

#include <libProgressReporter/ProgressReporter.hh>

#include "TrainingParameters.hh"
#include "iRoCSFeatures.hh"

class TrainEpidermisLabellingWorker : public QThread
{

  Q_OBJECT

  public:
  
  TrainEpidermisLabellingWorker(
      TrainingParameters const &parameters,
      iRoCS::ProgressReporter *progress = NULL);
  ~TrainEpidermisLabellingWorker();

  void run();

private:

  TrainingParameters const &_parameters;
  iRoCS::ProgressReporter *p_progress;
  
  double _sigmaMin, _sigmaMax, _sigmaStep;
  int _bandMax;

  iRoCS::Features *p_features;
  AnnotationChannelSpecs *p_annotation;

};

#endif
