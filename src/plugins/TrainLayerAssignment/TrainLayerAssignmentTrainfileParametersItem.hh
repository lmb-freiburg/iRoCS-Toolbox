#ifndef TRAINLAYERASSIGNMENTTRAINFILEPARAMETERSITEM_HH
#define TRAINLAYERASSIGNMENTTRAINFILEPARAMETERSITEM_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QTreeWidgetItem>

#include <libIRoCS/TrainfileParameters.hh>

#include <liblabelling_qt4/FileNameSelectionControlElement.hh>
#include <liblabelling_qt4/HDF5SelectionControlElement.hh>
#include <liblabelling_qt4/LabellingMainWidget.hh>

#include <string>

class TrainLayerAssignmentParametersDialog;

class TrainLayerAssignmentTrainfileParametersItem
    : public QTreeWidgetItem, public TrainfileParameters
{

public:

  TrainLayerAssignmentTrainfileParametersItem(
      LabellingMainWidget *mainWidget, std::string const &filename);
  ~TrainLayerAssignmentTrainfileParametersItem();
  
  void setTrainFileName(std::string const &name);
  std::string trainFileName() const;
  
  void setDataChannelName(std::string const &name);
  std::string dataChannelName() const;

  void setAnnotationChannelName(std::string const &name);
  std::string annotationChannelName() const;

  void setIRoCSChannelName(std::string const &name);
  std::string iRoCSChannelName() const;

  void setCacheFileName(std::string const &name);
  std::string cacheFileName() const;

  std::string check(int &nPositiveSamples, int &nSamples);

private:
  
  LabellingMainWidget *p_mainWidget;

  FileNameSelectionControlElement *p_trainFileSelector;
  HDF5SelectionControlElement *p_dataChannelControlElement;
  HDF5SelectionControlElement *p_annotationChannelControlElement;
  HDF5SelectionControlElement *p_iRoCSChannelControlElement;
  FileNameSelectionControlElement *p_cacheFileSelector;
  
  friend class TrainLayerAssignmentParametersDialog;

};

#endif
