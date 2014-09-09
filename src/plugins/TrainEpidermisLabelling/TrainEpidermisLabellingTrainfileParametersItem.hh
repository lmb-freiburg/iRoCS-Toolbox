#ifndef TRAINEPIDERMISLABELLINGTRAINFILEPARAMETERSITEM_HH
#define TRAINEPIDERMISLABELLINGTRAINFILEPARAMETERSITEM_HH

#include <QtGui/QTreeWidgetItem>

#include <libIRoCS/TrainfileParameters.hh>

#include <liblabelling_qt4/FileNameSelectionControlElement.hh>
#include <liblabelling_qt4/HDF5SelectionControlElement.hh>
#include <liblabelling_qt4/LabellingMainWidget.hh>

#include <string>

class TrainEpidermisLabellingParametersDialog;

class TrainEpidermisLabellingTrainfileParametersItem
    : public QTreeWidgetItem, public TrainfileParameters
{

public:

  TrainEpidermisLabellingTrainfileParametersItem(
      LabellingMainWidget *mainWidget, std::string const &filename);
  ~TrainEpidermisLabellingTrainfileParametersItem();
  
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
  
  friend class TrainEpidermisLabellingParametersDialog;

};

#endif
