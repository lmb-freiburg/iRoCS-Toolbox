/**************************************************************************
**       Title: Generic Marker Control Widget
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**************************************************************************/

#ifndef MARKERCONTROLWIDGET_HH
#define MARKERCONTROLWIDGET_HH

#include <QtGui/QGroupBox>

class QVBoxLayout;
class QScrollArea;

class AnnotationChannelSpecs;
class Marker;
class BoolControlElement;
class IntControlElement;
class StringControlElement;
class Double3ControlElement;
class DoubleVectorControlElement;
class IntDoubleMapControlElement;

class MarkerControlWidget : public QGroupBox 
{

  Q_OBJECT

public:

  MarkerControlWidget(AnnotationChannelSpecs* channel, QWidget* parent = 0);
  virtual ~MarkerControlWidget();
  
  virtual void getValues(Marker* marker) const;
  size_t featuresSize() const;

public slots:

  void resizeFeatures(int newSize);
  virtual void setValues(const Marker* marker);

signals:
  
  void contentsChanged();

protected slots:
  
  virtual void toggleReadOnly();

protected:
  
  AnnotationChannelSpecs* p_channel;

  QWidget* p_mainWidget;
  QVBoxLayout* p_layout;
  QScrollArea* p_scroller;

  BoolControlElement* p_showReadOnlyControl;
  StringControlElement* p_idControl;
  Double3ControlElement* p_positionControl;
  IntControlElement* p_labelControl;
  IntControlElement* p_predictedLabelControl;
  BoolControlElement* p_manualControl;
  BoolControlElement *p_needsFeatureUpdateControl;
  DoubleVectorControlElement* p_featuresControl;
  IntDoubleMapControlElement* p_probabilitiesControl;
  Double3ControlElement *p_iRoCSCoordinateControl;

};

#endif
