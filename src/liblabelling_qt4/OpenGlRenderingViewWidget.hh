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

#ifndef OPENGLRENDERINGVIEWWIDGET_HH
#define OPENGLRENDERINGVIEWWIDGET_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "ViewWidget.hh"

#include <libArrayToolbox/MarchingCubes.hh>
#include <libProgressReporter/ProgressReporterQt4.hh>

class QPushButton;

class OpenGlRenderingWidget;
class OpenGlRenderingSettingsWidget;

class OpenGlRenderingViewWidget : public ViewWidget
{

  Q_OBJECT

public:

  OpenGlRenderingViewWidget(
      MultiChannelModel* model, QWidget* parent = 0, Qt::WindowFlags f = 0);
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

signals:
        
  void visibilityStateChanged(bool);

public slots:

  virtual void redraw();
  void reinitializeAndUpdate();

private:

  void closeEvent(QCloseEvent *event);
  void showEvent(QShowEvent *event);
  void hideEvent(QHideEvent *event);

  OpenGlRenderingWidget *p_renderingWidget;
  OpenGlRenderingSettingsWidget *p_renderingSettings;
  QLabel *p_statusText;
  QProgressBar *p_progressBar;
  QPushButton *p_cancelButton;
  iRoCS::ProgressReporterQt4 *p_progress;

  friend class OpenGlRenderingWidget;

};

#endif
