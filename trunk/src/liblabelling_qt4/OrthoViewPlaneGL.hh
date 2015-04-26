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
