/**************************************************************************
 *
 * This file belongs to the iRoCS Toolbox.
 *
 * Copyright (C) 2015 Thorsten Falk
 *
 *        Image Analysis Lab, University of Freiburg, Germany
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 **************************************************************************/

#ifndef OPENGLRENDERINGSETTINGSWIDGET_HH
#define OPENGLRENDERINGSETTINGSWIDGET_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

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
