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

#include "OpenGlRenderingViewWidget.hh"

#include <QtGui/QVBoxLayout>
#include <QtGui/QToolBar>
#include <QtGui/QPushButton>

#include "OpenGlRenderingWidget.hh"
#include "OpenGlRenderingSettingsWidget.hh"

#include "MultiChannelModel.hh"
#include "IRoCSChannelSpecs.hh"
#include "IRoCSSCTChannelSpecs.hh"
#include "OrthoViewWidget.hh"

OpenGlRenderingViewWidget::OpenGlRenderingViewWidget(
    MultiChannelModel *model, OrthoViewWidget* orthoView, QWidget *parent,
    Qt::WindowFlags f)
        : ViewWidget(model, parent, f), p_orthoView(orthoView)
{
  p_renderingSettings = new OpenGlRenderingSettingsWidget(
      this, this, Qt::Window);
  p_renderingSettings->setWindowTitle(tr("iRoCS Toolbox - Rendering Settings"));
  p_renderingSettings->setVisible(isVisible());

  QVBoxLayout *layout = new QVBoxLayout;

  QToolBar *toolbar = new QToolBar;
  layout->addWidget(toolbar);

  p_renderingWidget = new OpenGlRenderingWidget(this);
  layout->addWidget(p_renderingWidget);

  toolbar->addAction(tr("Reset View"), p_renderingWidget, SLOT(resetView()));
  connect(p_renderingSettings, SIGNAL(renderingStateChanged()),
          SLOT(reinitializeAndUpdate()));
  connect(p_renderingSettings, SIGNAL(clippingStateChanged()),
          p_renderingWidget, SLOT(updateGL()));

  QHBoxLayout *progressLayout = new QHBoxLayout;

  p_statusText = new QLabel;
  p_statusText->setMinimumWidth(200);
  p_statusText->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  p_statusText->setVisible(false);
  progressLayout->addWidget(p_statusText);

  p_progressBar = new QProgressBar;
  p_progressBar->setFixedWidth(100);
  p_progressBar->setVisible(false);
  progressLayout->addWidget(p_progressBar);

  p_cancelButton = new QPushButton(
      QIcon::fromTheme("process-stop", QIcon(":/process-stop.png")), "");
  p_cancelButton->setVisible(false);
  progressLayout->addWidget(p_cancelButton);

  p_progress = new iRoCS::ProgressReporterQt4(
      p_progressBar, NULL, p_statusText, p_cancelButton);

  layout->addLayout(progressLayout);

  setLayout(layout);

  if (orthoView != NULL)
      connect(p_orthoView,
              SIGNAL(positionChanged(const blitz::TinyVector<double,3>&)),
              this, SLOT(redraw()));

  p_model->addView(this);
}

OpenGlRenderingViewWidget::~OpenGlRenderingViewWidget()
{
  p_model->removeView(this);
}

ViewWidget::ViewType OpenGlRenderingViewWidget::viewType() const
{
  return ViewWidget::OpenGlRenderingView;
}

void OpenGlRenderingViewWidget::addChannel(ChannelSpecs *)
{
  // Channels are only added to the MultiChannelModel,
  // never to the view directly
}

void OpenGlRenderingViewWidget::removeChannel(ChannelSpecs *)
{
  // Channels are implicitely removed from the view when removed from the
  // model
}

void OpenGlRenderingViewWidget::showProgressReporter(bool show)
{
  p_statusText->setVisible(show);
  p_progressBar->setVisible(show);
  p_cancelButton->setVisible(show);
}

iRoCS::ProgressReporterQt4 *OpenGlRenderingViewWidget::progressReporter()
{
  return p_progress;
}

double OpenGlRenderingViewWidget::latitudeSamplingUm() const
{
  return p_renderingSettings->latitudeSamplingUm();
}

int OpenGlRenderingViewWidget::longitudeSampling() const
{
  return p_renderingSettings->longitudeSampling();
}

bool OpenGlRenderingViewWidget::positionVisible(
    blitz::TinyVector<double,3> const &positionUm) const
{
  if (p_renderingSettings->clippingChannel() == NULL) return true;

  ChannelSpecs *clippingChannel = p_renderingSettings->clippingChannel();
  blitz::TinyVector<double,3> iRoCSPos(0.0);
  switch (clippingChannel->channelType())
  {
  case ChannelSpecs::IRoCS:
    iRoCSPos = static_cast<IRoCSChannelSpecs*>(
        clippingChannel)->rct()->getCoordinates(positionUm);
    break;
  case ChannelSpecs::IRoCSSCT:
    iRoCSPos = static_cast<IRoCSSCTChannelSpecs*>(
        clippingChannel)->sct()->getCoordinates(positionUm);
    break;
  default:
    return true;
  }

  if (iRoCSPos(0) < p_renderingSettings->zMin() ||
      iRoCSPos(0) > p_renderingSettings->zMax() ||
      iRoCSPos(1) < p_renderingSettings->rMin() ||
      iRoCSPos(1) > p_renderingSettings->rMax()) return false;

  if (p_renderingSettings->phiMin() > p_renderingSettings->phiMax())
  {
    if (iRoCSPos(2) > p_renderingSettings->phiMax() &&
        iRoCSPos(2) < p_renderingSettings->phiMin()) return false;
  }
  else
  {
    if (iRoCSPos(2) < p_renderingSettings->phiMin() ||
        iRoCSPos(2) > p_renderingSettings->phiMax()) return false;
  }
  return true;
}

OpenGlRenderingWidget *OpenGlRenderingViewWidget::renderingWidget()
{
  return p_renderingWidget;
}

OpenGlRenderingSettingsWidget *
OpenGlRenderingViewWidget::renderingSettingsWidget()
{
  return p_renderingSettings;
}

OrthoViewWidget *OpenGlRenderingViewWidget::orthoView() {
  return p_orthoView;
}

void OpenGlRenderingViewWidget::redraw()
{
#ifdef DEBUG
  std::cerr << "OpenGlRenderingViewWidget::redraw()" << std::endl;
#endif
  p_renderingWidget->updateGL();
}

void OpenGlRenderingViewWidget::reinitializeAndUpdate()
{
  if (isVisible())
  {
    p_renderingWidget->initializeGL();
    p_renderingWidget->updateGL();
  }
}

void OpenGlRenderingViewWidget::closeEvent(QCloseEvent *)
{
  p_renderingSettings->setVisible(false);
  emit visibilityStateChanged(false);
}

void OpenGlRenderingViewWidget::showEvent(QShowEvent *)
{
  p_renderingSettings->setVisible(true);
}

void OpenGlRenderingViewWidget::hideEvent(QHideEvent *)
{
  p_renderingSettings->setVisible(false);
}
