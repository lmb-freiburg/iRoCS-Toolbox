#ifndef ASSIGNLAYERSTOCELLSEGMENTATIONPARAMETERSDIALOG_HH
#define ASSIGNLAYERSTOCELLSEGMENTATIONPARAMETERSDIALOG_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QDialog>

#include <liblabelling_qt4/ChannelSelectionControlElement.hh>
#include <liblabelling_qt4/FileNameSelectionControlElement.hh>
#include <liblabelling_qt4/StringControlElement.hh>
#include <liblabelling_qt4/IntControlElement.hh>
#include <liblabelling_qt4/DoubleControlElement.hh>
#include <liblabelling_qt4/LabellingMainWidget.hh>

#include <liblabelling_qt4/VisualizationChannelSpecs.hh>
#include <liblabelling_qt4/IRoCSSCTChannelSpecs.hh>

class AssignLayersToCellSegmentationParametersDialog : public QDialog
{

  Q_OBJECT

  public:
  
  AssignLayersToCellSegmentationParametersDialog(
      LabellingMainWidget* mainWidget,
      QWidget* parent = NULL, Qt::WindowFlags f = NULL);
  ~AssignLayersToCellSegmentationParametersDialog();

  VisualizationChannelSpecs *segmentationChannel() const;
  void setSegmentationChannel(VisualizationChannelSpecs *channel);

  IRoCSSCTChannelSpecs *iRoCSChannel() const;
  void setIRoCSChannel(IRoCSSCTChannelSpecs *channel);

  std::string outFileName() const;
  void setOutFileName(std::string const &outFileName);

  std::string labelName() const;
  void setLabelName(std::string const &labelName);

  std::string modelFileName() const;
  void setModelFileName(std::string const &modelFileName);

  std::string featureFileName() const;
  void setFeatureFileName(std::string const &featureFileName);

  std::string featureGroup() const;
  void setFeatureGroup(std::string const &featureGroup);

  double volumeThresholdUm3() const;
  void setVolumeThresholdUm3(double volumeThresholdUm3);

  int backgroundLabel() const;
  void setBackgroundLabel(int backgroundLabel);

  std::string check();

public slots:
  
  void checkAndAccept();

private:
  
  LabellingMainWidget *p_mainWidget;

  ChannelSelectionControlElement *p_segmentationChannelSelector;
  ChannelSelectionControlElement *p_iRoCSChannelSelector;
  FileNameSelectionControlElement *p_outFileControlElement;
  StringControlElement *p_labelNameControlElement;
  FileNameSelectionControlElement *p_modelFileControlElement;
  FileNameSelectionControlElement *p_featureFileControlElement;
  StringControlElement *p_featureGroupControlElement;

  DoubleControlElement *p_volumeThresholdUm3Control;
  IntControlElement *p_backgroundLabelControl;

};

#endif
