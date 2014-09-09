#ifndef ATTACHIROCSTOCELLSEGMENTATIONPARAMETERSDIALOG_HH
#define ATTACHIROCSTOCELLSEGMENTATIONPARAMETERSDIALOG_HH

#include <QtGui/QDialog>

#include <libIRoCS/AttachIRoCSToCellSegmentationParameters.hh>

#include <liblabelling_qt4/ChannelSelectionControlElement.hh>
#include <liblabelling_qt4/StringControlElement.hh>
#include <liblabelling_qt4/IntControlElement.hh>
#include <liblabelling_qt4/DoubleControlElement.hh>
#include <liblabelling_qt4/LabellingMainWidget.hh>

class AttachIRoCSToCellSegmentationParametersDialog :
    public QDialog, public AttachIRoCSToCellSegmentationParameters
{

  Q_OBJECT

  public:
  
  AttachIRoCSToCellSegmentationParametersDialog(
      LabellingMainWidget* mainWidget,
      QWidget* parent = NULL, Qt::WindowFlags f = NULL);
  ~AttachIRoCSToCellSegmentationParametersDialog();

  VisualizationChannelSpecs *segmentationChannel() const;
  void setSegmentationChannel(VisualizationChannelSpecs *channel);

  AnnotationChannelSpecs *qcChannel() const;
  void setQcChannel(AnnotationChannelSpecs *channel);

  std::string iRoCSChannelName() const;
  void setIRoCSChannelName(std::string const &name);

  double dataWeight() const;
  void setDataWeight(double kappa);

  double axisStiffnessWeight() const;
  void setAxisStiffnessWeight(double lambda);

  double thicknessConstancyWeight() const;
  void setThicknessConstancyWeight(double mu);

  int nIterations() const;
  void setNIterations(int nIter);

  double optimizationTimeStep() const;
  void setOptimizationTimeStep(double tau);

public slots:
  
  void checkAndAccept();

private:
  
  LabellingMainWidget *p_mainWidget;

  ChannelSelectionControlElement *p_segmentationChannelSelector;
  ChannelSelectionControlElement *p_qcChannelSelector;
  StringControlElement *p_iRoCSChannelNameControlElement;

  DoubleControlElement *p_kappaControl;
  DoubleControlElement *p_lambdaControl;
  DoubleControlElement *p_muControl;
  IntControlElement *p_nIterControl;
  DoubleControlElement *p_tauControl;

};

#endif
