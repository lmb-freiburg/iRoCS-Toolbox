/**************************************************************************
**       Title: Settings widget for OpenGL renderung view
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    Settings widget for OpenGL renderung view
**
**************************************************************************/

#ifndef OPENGLRENDERINGSETTINGSWIDGET_HH
#define OPENGLRENDERINGSETTINGSWIDGET_HH

#include <QtGui/QWidget>

class OpenGlRenderingViewWidget;
class StringSelectionControlElement;
class BoolControlElement;
class DoubleControlElement;
class IntControlElement;
class ChannelSelectionControlElement;
class DoubleRangeControlElement;
class QDial;

class ChannelSpecs;

class OpenGlRenderingSettingsWidget : public QWidget
{

  Q_OBJECT
  
  public:
  
  enum RenderingState { Solid = 0, Wireframe = 1, Dots = 2 };

  OpenGlRenderingSettingsWidget(
      OpenGlRenderingViewWidget *view, QWidget *parent = 0,
      Qt::WindowFlags f = 0);
  
  ~OpenGlRenderingSettingsWidget();

  RenderingState frontFaceRendering() const;
  RenderingState backFaceRendering() const;
  bool backFaceCullingEnabled() const;
  bool lightEnabled() const;
  float materialShininess() const;
  double latitudeSamplingUm() const;
  int longitudeSampling() const;

  ChannelSpecs *clippingChannel() const;
  double zMin() const;
  double zMax() const;
  double rMin() const;
  double rMax() const;
  double phiMin() const;
  double phiMax() const;

signals:
  
  // This signal indicates, that the OpenGl renderer needs to be reinitialized
  // and should trigger an intializeGL() and updateGL()
  void renderingStateChanged();

  // This signal is emitted, when the content to render changed, therefore
  // it only needs to trigger an updateGL()
  void clippingStateChanged();

private slots:
  
  void _setLiveUpdateEnabled(bool enable);

private:

  OpenGlRenderingViewWidget *p_view;

  BoolControlElement *p_liveUpdateControl;
  StringSelectionControlElement *p_frontFaceRenderingControl;
  StringSelectionControlElement *p_backFaceRenderingControl;
  BoolControlElement *p_backFaceCullingControl;
  BoolControlElement *p_lightControl;
  DoubleControlElement *p_materialShininessControl;
  DoubleControlElement *p_latitudeSamplingControl;
  IntControlElement *p_longitudeSamplingControl;

  ChannelSelectionControlElement *p_channelControl;
  DoubleRangeControlElement *p_zRangeControl;
  DoubleRangeControlElement *p_rRangeControl;
  QDial *p_phiMin, *p_phiMax;

};

#endif
