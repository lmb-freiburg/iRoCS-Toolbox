/**************************************************************************
**       Title: OpenGL 2-D color viewer widget
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    Widget for displaying 2-D RGB data in a QGLWidget
**
**************************************************************************/

#ifndef ORTHOVIEWPLANEGL_HH
#define ORTHOVIEWPLANEGL_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <blitz/array.h>

class QGLWidget;
class QMouseEvent;
class QWheelEvent;

class OrthoViewWidget;

class OrthoViewPlaneGL : public QGLWidget 
{

  Q_OBJECT

public:
  
  OrthoViewPlaneGL(OrthoViewWidget* orthoView,
                   const blitz::TinyVector<int,2>& dimensions,
                   QWidget* parent = 0);
  ~OrthoViewPlaneGL();
  
  void clearData();
  void addData(const blitz::Array<blitz::TinyVector<float,3>,2>& data);

signals:

  void mouseMoved(ptrdiff_t x, ptrdiff_t y);
  void mouseWheelMoved(ptrdiff_t z);
  void crosshairDragged(ptrdiff_t x, ptrdiff_t y);
  void hLineDragged(ptrdiff_t x);
  void vLineDragged(ptrdiff_t y);
  void positionSelected(
      ptrdiff_t x, ptrdiff_t y,
      Qt::MouseButton button, Qt::KeyboardModifiers modifiers);

private:
  
  double elSizeZoom(int dim) const;
  double zoom() const;
  ptrdiff_t pos(int dim) const;

  void initializeGL();
  void resizeGL(int w, int h);
  void paintGL();

  void mouseMoveEvent(QMouseEvent* e);
  void mousePressEvent(QMouseEvent* e);
  void mouseReleaseEvent(QMouseEvent* e);
  void wheelEvent(QWheelEvent* e);

  OrthoViewWidget* p_orthoView;
  blitz::Array<blitz::TinyVector<float,3>,2> _data;
  blitz::TinyVector<int,2> _dimensions;
  GLuint _texture;

  bool _dragVertical, _dragHorizontal, _isDragging;

};

#endif
