#ifndef TRAINDETECTORWORKER_HH
#define TRAINDETECTORWORKER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtCore/QThread>

#include <libProgressReporter/ProgressReporter.hh>

#include "TrainingParameters.hh"
#include "iRoCSFeatures.hh"

class TrainDetectorWorker : public QThread
{

  Q_OBJECT

  public:
  
  TrainDetectorWorker(
      TrainingParameters const &parameters,
      iRoCS::ProgressReporter *progress = NULL);
  ~TrainDetectorWorker();

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
