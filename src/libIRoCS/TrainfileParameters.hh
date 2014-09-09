#ifndef TRAINFILEPARAMETERS_HH
#define TRAINFILEPARAMETERS_HH

#include <string>

class TrainfileParameters
{

public:

  TrainfileParameters(bool needsIRoCSChannel = false);
  virtual ~TrainfileParameters();
  
  void setNeedsIRoCSChannel(bool needsIRoCSChannel);
  bool needsIRoCSChannel() const;

  virtual void setTrainFileName(std::string const &name);
  virtual std::string trainFileName() const;

  virtual void setDataChannelName(std::string const &name);
  virtual std::string dataChannelName() const;

  virtual void setAnnotationChannelName(std::string const &name);
  virtual std::string annotationChannelName() const;

  virtual void setIRoCSChannelName(std::string const &name);
  virtual std::string iRoCSChannelName() const;

  virtual void setCacheFileName(std::string const &name);
  virtual std::string cacheFileName() const;

  virtual std::string check(int &nPositiveSamples, int &nSamples);

private:
  
  bool _needsIRoCSChannel;
  std::string _trainFileName, _dataChannelName;
  std::string _annotationChannelName, _iRoCSChannelName, _cacheFileName;

};

#endif
