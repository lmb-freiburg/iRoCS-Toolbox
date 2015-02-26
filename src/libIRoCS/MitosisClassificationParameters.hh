#ifndef MITOSISCLASSIFICATIONPARAMETERS_HH
#define MITOSISCLASSIFICATIONPARAMETERS_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <string>

#include <liblabelling_qt4/DataChannelSpecs.hh>
#include <liblabelling_qt4/AnnotationChannelSpecs.hh>

class MitosisClassificationParameters
{

public:
  
  MitosisClassificationParameters();
  virtual ~MitosisClassificationParameters();

  virtual DataChannelSpecs *dataChannel() const;
  virtual void setDataChannel(DataChannelSpecs *channel);

  virtual AnnotationChannelSpecs *annotationChannel() const;
  virtual void setAnnotationChannel(AnnotationChannelSpecs *channel);

  virtual std::string cacheFileName() const;
  virtual void setCacheFileName(std::string const &cacheFileName);

  virtual std::string modelFileName() const;
  virtual void setModelFileName(std::string const &modelFileName);

  std::string check();

private:
  
  DataChannelSpecs *p_dataChannel;
  AnnotationChannelSpecs *p_annotationChannel;
  std::string _cacheFileName;
  std::string _modelFileName;

};

#endif
