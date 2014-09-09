#ifndef LAYERASSIGNMENTPARAMETERSDIALOG_HH
#define LAYERASSIGNMENTPARAMETERSDIALOG_HH

#include <QtGui/QDialog>

#include <liblabelling_qt4/ChannelSelectionControlElement.hh>
#include <liblabelling_qt4/FileNameSelectionControlElement.hh>
#include <liblabelling_qt4/BoolControlElement.hh>
#include <liblabelling_qt4/ChannelSpecs.hh>
#include <liblabelling_qt4/AnnotationChannelSpecs.hh>
#include <liblabelling_qt4/IRoCSChannelSpecs.hh>
#include <liblabelling_qt4/LabellingMainWidget.hh>

#include <string>

class LayerAssignmentParametersDialog :
    public QDialog
{

  Q_OBJECT

  public:
  
  LayerAssignmentParametersDialog(
      LabellingMainWidget* mainWidget,
      QWidget* parent = NULL, Qt::WindowFlags f = NULL);
  ~LayerAssignmentParametersDialog();

  ChannelSpecs *dataChannel() const;
  void setDataChannel(ChannelSpecs *channel);

  AnnotationChannelSpecs *annotationChannel() const;
  void setAnnotationChannel(AnnotationChannelSpecs *channel);

  IRoCSChannelSpecs *iRoCSChannel() const;
  void setIRoCSChannel(IRoCSChannelSpecs *channel);

  std::string cacheFileName() const;
  void setCacheFileName(std::string const &cacheFileName);

  std::string modelFileName() const;
  void setModelFileName(std::string const &modelFileName);

  bool cacheCoordinates() const;
  void setCacheCoordinates(bool cache);

  bool updateMitoses() const;
  void setUpdateMitoses(bool update);

  bool forceFeatureComputation() const;
  void setForceFeatureComputation(bool force);

  std::string check();

public slots:
  
  void checkAndAccept();

private:
  
  LabellingMainWidget* p_mainWidget;

  ChannelSelectionControlElement *p_dataChannelSelector;
  ChannelSelectionControlElement *p_annotationChannelSelector;
  ChannelSelectionControlElement *p_iRoCSChannelSelector;
  FileNameSelectionControlElement *p_cacheFileControlElement;
  FileNameSelectionControlElement *p_modelFileNameControlElement;
  BoolControlElement *p_cacheCoordinates;
  BoolControlElement *p_updateMitoses;
  BoolControlElement *p_forceFeatureComputationControl;

};

#endif
