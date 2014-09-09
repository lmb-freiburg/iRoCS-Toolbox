#ifndef TRAINLAYERASSIGNMENTWORKER_HH
#define TRAINLAYERASSIGNMENTWORKER_HH

#include <QtCore/QThread>

#include <libProgressReporter/ProgressReporter.hh>

#include "TrainingParameters.hh"
#include "iRoCSFeatures.hh"

class TrainLayerAssignmentWorker : public QThread
{

  Q_OBJECT

  public:
  
  TrainLayerAssignmentWorker(
      TrainingParameters const &parameters,
      iRoCS::ProgressReporter *progress = NULL);
  ~TrainLayerAssignmentWorker();

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
