#ifndef TRAINDETECTORTRAINFILEPARAMETERSITEM_HH
#define TRAINDETECTORTRAINFILEPARAMETERSITEM_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QTreeWidgetItem>

#include <libIRoCS/TrainfileParameters.hh>

#include <liblabelling_qt4/FileNameSelectionControlElement.hh>
#include <liblabelling_qt4/HDF5SelectionControlElement.hh>
#include <liblabelling_qt4/LabellingMainWidget.hh>

#include <string>

class TrainDetectorParametersDialog;

class TrainDetectorTrainfileParametersItem
    : public QTreeWidgetItem, public TrainfileParameters
{

public:

  TrainDetectorTrainfileParametersItem(
      LabellingMainWidget *mainWidget, std::string const &filename);
  ~TrainDetectorTrainfileParametersItem();
  
  void setTrainFileName(std::string const &name);
  std::string trainFileName() const;
  
  void setDataChannelName(std::string const &name);
  std::string dataChannelName() const;

  void setAnnotationChannelName(std::string const &name);
  std::string annotationChannelName() const;

  void setCacheFileName(std::string const &name);
  std::string cacheFileName() const;

  std::string check(int &nPositiveSamples, int &nSamples);

private:
  
  LabellingMainWidget *p_mainWidget;

  FileNameSelectionControlElement *p_trainFileSelector;
  HDF5SelectionControlElement *p_dataChannelControlElement;
  HDF5SelectionControlElement *p_annotationChannelControlElement;
  FileNameSelectionControlElement *p_cacheFileSelector;
  
  friend class TrainDetectorParametersDialog;

};

#endif
