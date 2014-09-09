#ifndef TRAININGPARAMETERS_HH
#define TRAININGPARAMETERS_HH

#include <string>
#include <vector>

#include "iRoCSFeatures.hh"

class TrainfileParameters;

class TrainingParameters
{

public:
  
  TrainingParameters();
  virtual ~TrainingParameters();

  void addTrainFile(TrainfileParameters* parameters);
  virtual std::vector<TrainfileParameters*> trainFiles() const;

  virtual void setFeatureGroup(std::string const &group);
  virtual std::string featureGroup() const;

  virtual void setGenerateRandomSamples(bool generate);
  virtual bool generateRandomSamples() const;

  virtual void setNInRootSamples(int samples);
  virtual int nInRootSamples() const;

  virtual void setNOutRootSamples(int samples);
  virtual int nOutRootSamples() const;

  virtual void setModelFileName(std::string const &name);
  virtual std::string modelFileName() const;

  virtual void setSdFeatureNormalization(
      iRoCS::Features::NormalizationType normType);
  virtual iRoCS::Features::NormalizationType sdFeatureNormalization() const;

  virtual void setHoughFeatureNormalization(
      iRoCS::Features::NormalizationType normType);
  virtual iRoCS::Features::NormalizationType houghFeatureNormalization() const;

  virtual void setCost(double cost);
  virtual double cost() const;

  virtual void setGamma(double gamma);
  virtual double gamma() const;

  std::string check();

private:

  std::vector<TrainfileParameters*> _trainFiles;
  std::string _featureGroup;
  bool _generateRandomSamples;
  int _nInRootSamples, _nOutRootSamples;
  std::string _modelFileName;
  iRoCS::Features::NormalizationType _sdNormalization, _houghNormalization;
  double _cost, _gamma;
  
};

#endif
