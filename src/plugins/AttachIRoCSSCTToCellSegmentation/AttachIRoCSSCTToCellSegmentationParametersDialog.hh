#ifndef ATTACHIROCSSCTTOCELLSEGMENTATIONPARAMETERSDIALOG_HH
#define ATTACHIROCSSCTTOCELLSEGMENTATIONPARAMETERSDIALOG_HH

#include <QtGui/QDialog>

#include <liblabelling_qt4/AnnotationChannelSpecs.hh>
#include <liblabelling_qt4/ChannelSelectionControlElement.hh>
#include <liblabelling_qt4/StringControlElement.hh>
#include <liblabelling_qt4/IntControlElement.hh>
#include <liblabelling_qt4/DoubleControlElement.hh>
#include <liblabelling_qt4/LabellingMainWidget.hh>

class AttachIRoCSSCTToCellSegmentationParametersDialog : public QDialog
{

  Q_OBJECT

  public:
  
  AttachIRoCSSCTToCellSegmentationParametersDialog(
      LabellingMainWidget* mainWidget,
      QWidget* parent = NULL, Qt::WindowFlags f = NULL);
  ~AttachIRoCSSCTToCellSegmentationParametersDialog();

  VisualizationChannelSpecs *segmentationChannel() const;
  void setSegmentationChannel(VisualizationChannelSpecs *channel);

  AnnotationChannelSpecs *qcChannel() const;
  void setQcChannel(AnnotationChannelSpecs *channel);

  std::string iRoCSChannelName() const;
  void setIRoCSChannelName(std::string const &name);

  int backgroundLabel() const;
  void setBackgroundLabel(int backgroundLabel);

  double segmentLengthUm() const;
  void setSegmentLengthUm(double segmentLengthUm);

  double marginUm() const;
  void setMarginUm(double marginUm);

  int downsampleRatio() const;
  void setDownsampleRatio(int downsampleRatio);

  std::string check();

public slots:
  
  void checkAndAccept();

private:
  
  LabellingMainWidget *p_mainWidget;

  ChannelSelectionControlElement *p_segmentationChannelSelector;
  ChannelSelectionControlElement *p_qcChannelSelector;
  StringControlElement *p_iRoCSChannelNameControlElement;

  IntControlElement *p_backgroundLabelControl;
  DoubleControlElement *p_segmentLengthUmControl;
  DoubleControlElement *p_marginUmControl;
  IntControlElement *p_downsampleRatioControl;

};

#endif
