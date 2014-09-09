#ifndef MITOSISCLASSIFICATIONWORKER_HH
#define MITOSISCLASSIFICATIONWORKER_HH

#include <QtCore/QThread>

#include <liblabelling_qt4/LabellingMainWidget.hh>

#include <libProgressReporter/ProgressReporter.hh>

#include <libsvmtl/BasicFV.hh>

#include "MitosisClassificationParameters.hh"
#include "iRoCSFeatures.hh"

class MitosisClassificationWorker : public QThread
{

  Q_OBJECT

  public:
  
  MitosisClassificationWorker(
      MitosisClassificationParameters const &parameters,
      iRoCS::ProgressReporter *progress = NULL,
      LabellingMainWidget* mainWidget = NULL,
      QWidget* parent = NULL);
  ~MitosisClassificationWorker();

  void run();

private:

  MitosisClassificationParameters const & _parameters;
  iRoCS::ProgressReporter *p_progress;
  LabellingMainWidget* p_mainWidget;

  double _sigmaMin, _sigmaMax, _sigmaStep;
  int _bandMax;

  iRoCS::Features *p_features;

  std::vector<svt::BasicFV> _testVectors;

};

#endif
