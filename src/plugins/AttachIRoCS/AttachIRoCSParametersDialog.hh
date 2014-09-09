#ifndef ATTACHIROCSPARAMETERSDIALOG_HH
#define ATTACHIROCSPARAMETERSDIALOG_HH

#include <QtGui/QDialog>

#include <liblabelling_qt4/AnnotationChannelSpecs.hh>
#include <liblabelling_qt4/ChannelSelectionControlElement.hh>
#include <liblabelling_qt4/StringControlElement.hh>
#include <liblabelling_qt4/IntControlElement.hh>
#include <liblabelling_qt4/DoubleControlElement.hh>
#include <liblabelling_qt4/LabellingMainWidget.hh>

class AttachIRoCSParametersDialog : public QDialog
{

  Q_OBJECT

  public:
  
  AttachIRoCSParametersDialog(
      LabellingMainWidget* mainWidget,
      QWidget* parent = NULL, Qt::WindowFlags f = NULL);
  ~AttachIRoCSParametersDialog();

  AnnotationChannelSpecs *markerChannel() const;
  void setMarkerChannel(AnnotationChannelSpecs *channel);

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

  double searchRadiusUm() const;
  void setSearchRadiusUm(double searchRadiusUm);

  int nIterations() const;
  void setNIterations(int nIter);

  double optimizationTimeStep() const;
  void setOptimizationTimeStep(double tau);

  std::string check();

public slots:
  
  void checkAndAccept();

private:
  
  LabellingMainWidget *p_mainWidget;

  ChannelSelectionControlElement *p_markerChannelSelector;
  ChannelSelectionControlElement *p_qcChannelSelector;
  StringControlElement *p_iRoCSChannelNameControlElement;

  DoubleControlElement *p_kappaControl;
  DoubleControlElement *p_lambdaControl;
  DoubleControlElement *p_muControl;
  IntControlElement *p_nIterControl;
  DoubleControlElement *p_tauControl;
  DoubleControlElement *p_searchRadiusUmControl;

};

#endif
