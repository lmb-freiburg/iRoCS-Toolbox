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

#ifndef OPENGLRENDERINGWIDGET_HH
#define OPENGLRENDERINGWIDGET_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtOpenGL/QGLWidget>
#include <QtGui/QMouseEvent>

#include <blitz/array.h>

class OpenGlRenderingViewWidget;

class OpenGlRenderingWidget : public QGLWidget
{

  Q_OBJECT

public:

  OpenGlRenderingWidget(OpenGlRenderingViewWidget *view = NULL,
                        QWidget *parent = NULL);
  ~OpenGlRenderingWidget();
  
public slots:
  
  void resetView();

private:
  
  void initializeGL();
  void resizeGL(int width, int height);
  void paintGL();

  void mousePressEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void wheelEvent(QWheelEvent *event);

  OpenGlRenderingViewWidget *p_view;

  QPoint _lastMousePosition;
  blitz::TinyMatrix<double,4,4> _rotation;
  blitz::TinyVector<float,2> _translation;
  float _distanceToOrigin;

  friend class OpenGlRenderingViewWidget;

};

#endif
