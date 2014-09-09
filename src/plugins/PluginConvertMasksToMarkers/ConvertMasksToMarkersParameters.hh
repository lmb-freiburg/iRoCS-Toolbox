#ifndef CONVERTMASKSTOMARKERSPARAMETERS_HH
#define CONVERTMASKSTOMARKERSPARAMETERS_HH

#include <QtGui/QDialog>

#include <liblabelling_qt4/Marker.hh>

class LabellingMainWidget;
class VisualizationChannelSpecs;
class ChannelSelectionControlElement;
class StringControlElement;
class StringSelectionControlElement;
class DoubleControlElement;
class BoolControlElement;
class PluginConvertMasksToMarkers;

class ConvertMasksToMarkersParameters : public QDialog
{

  Q_OBJECT

  public:
  
  ConvertMasksToMarkersParameters(LabellingMainWidget* mainWidget,
                                  QWidget* parent = 0, Qt::WindowFlags f = 0);
  ~ConvertMasksToMarkersParameters();

  VisualizationChannelSpecs *maskChannel() const;
  std::string annotationChannelName() const;
  bool doConnectedComponentLabelling() const;
  Marker::MarkerType markerType() const;
  double smoothingSigmaPx() const;
  double simplifyToleranceUm3() const;

protected slots:
  
  void checkAndAccept();

private:
  
  LabellingMainWidget* p_mainWidget;

  ChannelSelectionControlElement* p_maskChannelSelector;
  StringControlElement* p_annotationChannelControlElement;
  BoolControlElement *p_doConnectedComponentLabellingControlElement;
  StringSelectionControlElement* p_markerTypeControlElement;
  DoubleControlElement *p_smoothingSigmaPxControl;
  DoubleControlElement *p_simplifyToleranceUm3Control;

  friend class PluginConvertMasksToMarkers;

};

#endif
