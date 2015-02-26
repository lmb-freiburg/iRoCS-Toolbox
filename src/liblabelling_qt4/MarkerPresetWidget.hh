/**************************************************************************
**       Title: Generic Marker Preset Widget
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**************************************************************************/

#ifndef MARKERPRESETWIDGET_HH
#define MARKERPRESETWIDGET_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QGroupBox>

class AnnotationChannelSpecs;
class Marker;
class IntControlElement;

class QVBoxLayout;
class QDoubleSpinBox;
class QPushButton;

class MarkerPresetWidget : public QGroupBox
{
  
  Q_OBJECT

public:

  MarkerPresetWidget(AnnotationChannelSpecs* channel, QWidget* parent = 0);
  virtual ~MarkerPresetWidget();

  virtual void getValues(Marker* marker) const;
  virtual void setValues(const Marker* marker);

signals:
  
  void colormapChanged();

protected slots:
  
  void updateAlpha();
  void chooseColor();
  void updateLabelColorPushButton();

protected:
  
  AnnotationChannelSpecs* p_channel;

  QVBoxLayout* p_layout;
  IntControlElement* p_labelControl;
  QDoubleSpinBox *p_alphaSpinBox;
  QPushButton* p_labelColorPushButton;

};

#endif
