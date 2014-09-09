#ifndef DETECTSPHERESPARAMETERSDIALOG_HH
#define DETECTSPHERESPARAMETERSDIALOG_HH

#include <QtGui/QDialog>

#include <liblabelling_qt4/ChannelSpecs.hh>
#include <liblabelling_qt4/ChannelSelectionControlElement.hh>
#include <liblabelling_qt4/StringControlElement.hh>
#include <liblabelling_qt4/DoubleRangeControlElement.hh>
#include <liblabelling_qt4/DoubleControlElement.hh>
#include <liblabelling_qt4/LabellingMainWidget.hh>
#include <liblabelling_qt4/BoolControlElement.hh>

#include <string>

class DetectSpheresParametersDialog : public QDialog
{

  Q_OBJECT

  public:
  
  DetectSpheresParametersDialog(
      LabellingMainWidget* mainWidget,
      QWidget* parent = NULL, Qt::WindowFlags f = NULL);
  ~DetectSpheresParametersDialog();

  ChannelSpecs* dataChannel() const;
  void setDataChannel(ChannelSpecs *channel);

  std::string annotationChannelName() const;
  void setAnnotationChannelName(std::string const &name);

  blitz::TinyVector<double,2> radiusRangeUm() const;
  void setRadiusRangeUm(blitz::TinyVector<double,2> const &range);
  
  double radiusStepUm() const;
  void setRadiusStepUm(double radiusStepUm);

  double preSmoothingSigmaUm() const;
  void setPreSmoothingSigmaUm(double sigmaUm);

  double postSmoothingSigmaUm() const;
  void setPostSmoothingSigmaUm(double sigmaUm);

  double minMagnitude() const;
  void setMinMagnitude(double minMagnitude);

  bool invertGradients() const;
  void setInvertGradients(bool invert);

  double gamma() const;
  void setGamma(double gamma);

  std::string check();

public slots:
  
  void checkAndAccept();

private:
  
  LabellingMainWidget* p_mainWidget;

  ChannelSelectionControlElement *p_dataChannelSelector;
  StringControlElement *p_annotationChannelControlElement;
  DoubleRangeControlElement *p_radiusRangeControl;
  DoubleControlElement *p_radiusStepControl;
  DoubleControlElement *p_preSmoothingSigmaUmControl;
  DoubleControlElement *p_postSmoothingSigmaUmControl;
  DoubleControlElement *p_minMagnitudeControl;
  BoolControlElement *p_invertGradientsControl;
  DoubleControlElement *p_gammaControl;

};

#endif
