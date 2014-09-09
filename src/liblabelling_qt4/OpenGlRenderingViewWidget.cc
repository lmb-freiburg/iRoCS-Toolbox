/**************************************************************************
**       Title: Volume visualization widget using OpenGL
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    3D Rendering view of a MultiChannelModel
**
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

OpenGlRenderingViewWidget::OpenGlRenderingViewWidget(
    MultiChannelModel *model, QWidget *parent,  Qt::WindowFlags f)
        : ViewWidget(model, parent, f)
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
