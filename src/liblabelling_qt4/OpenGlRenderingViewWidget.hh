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

#ifndef OPENGLRENDERINGVIEWWIDGET_HH
#define OPENGLRENDERINGVIEWWIDGET_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "ViewWidget.hh"

#include <libArrayToolbox/MarchingCubes.hh>
#include <libProgressReporter_qt4/ProgressReporterQt4.hh>

class QPushButton;

class OrthoViewWidget;
class OpenGlRenderingWidget;
class OpenGlRenderingSettingsWidget;

class OpenGlRenderingViewWidget : public ViewWidget
{

  Q_OBJECT

public:

  OpenGlRenderingViewWidget(
      MultiChannelModel* model, OrthoViewWidget* orthoView = NULL,
      QWidget* parent = NULL, Qt::WindowFlags f = 0);
  ~OpenGlRenderingViewWidget();

  ViewWidget::ViewType viewType() const;

  void addChannel(ChannelSpecs *channel);
  void removeChannel(ChannelSpecs* channel);

  void showProgressReporter(bool show);
  iRoCS::ProgressReporterQt4 *progressReporter();

  double latitudeSamplingUm() const;
  int longitudeSampling() const;

  bool positionVisible(blitz::TinyVector<double,3> const &positionUm) const;

  OpenGlRenderingWidget *renderingWidget();
  OpenGlRenderingSettingsWidget *renderingSettingsWidget();

  OrthoViewWidget *orthoView();

signals:

  void visibilityStateChanged(bool);

public slots:

  virtual void redraw();
  void reinitializeAndUpdate();

private:

  void closeEvent(QCloseEvent *event);
  void showEvent(QShowEvent *event);
  void hideEvent(QHideEvent *event);

  OrthoViewWidget *p_orthoView;
  OpenGlRenderingWidget *p_renderingWidget;
  OpenGlRenderingSettingsWidget *p_renderingSettings;
  QLabel *p_statusText;
  QProgressBar *p_progressBar;
  QPushButton *p_cancelButton;
  iRoCS::ProgressReporterQt4 *p_progress;

  friend class OpenGlRenderingWidget;

};

#endif
