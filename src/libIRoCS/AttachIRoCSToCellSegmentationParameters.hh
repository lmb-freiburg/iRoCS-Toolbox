#ifndef ATTACHIROCSTOCELLSEGMENTATIONPARAMETERS_HH
#define ATTACHIROCSTOCELLSEGMENTATIONPARAMETERS_HH

#include <string>

#include <liblabelling_qt4/AnnotationChannelSpecs.hh>

class AttachIRoCSToCellSegmentationParameters
{

public:
  
  AttachIRoCSToCellSegmentationParameters();
  virtual ~AttachIRoCSToCellSegmentationParameters();

  virtual VisualizationChannelSpecs *segmentationChannel() const;
  virtual void setSegmentationChannel(VisualizationChannelSpecs *channel);

  virtual AnnotationChannelSpecs *qcChannel() const;
  virtual void setQcChannel(AnnotationChannelSpecs *channel);

  virtual std::string iRoCSChannelName() const;
  virtual void setIRoCSChannelName(std::string const &name);

  virtual double dataWeight() const;
  virtual void setDataWeight(double kappa);

  virtual double axisStiffnessWeight() const;
  virtual void setAxisStiffnessWeight(double lambda);

  virtual double thicknessConstancyWeight() const;
  virtual void setThicknessConstancyWeight(double mu);

  virtual int nIterations() const;
  virtual void setNIterations(int nIter);

  virtual double optimizationTimeStep() const;
  virtual void setOptimizationTimeStep(double tau);

  std::string check();

private:
  
  VisualizationChannelSpecs *p_segmentationChannel;
  AnnotationChannelSpecs *p_qcChannel;
  std::string _iRoCSChannelName;
  double _kappa, _lambda, _mu, _tau;
  int _nIter;

};

#endif
