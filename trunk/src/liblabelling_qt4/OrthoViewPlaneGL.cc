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

#include "OrthoViewPlaneGL.hh"

#include <QtOpenGL/QGLWidget>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>

#include "OrthoViewWidget.hh"

// #include <blitz/tinyvec-et.h>

OrthoViewPlaneGL::OrthoViewPlaneGL(
    OrthoViewWidget* orthoView, 
    const blitz::TinyVector<int,2>& dimensions,
    QWidget* parent)
        : QGLWidget(parent), p_orthoView(orthoView),
          _dimensions(dimensions),
          _dragVertical(false), _dragHorizontal(false),
          _isDragging(false)
{
  setMouseTracking(true);
  setFixedSize(0, 0);
}

OrthoViewPlaneGL::~OrthoViewPlaneGL() 
{
  glDeleteTextures(1, &_texture);
}
  
void OrthoViewPlaneGL::clearData() 
{
  _data = blitz::TinyVector<float,3>(0.0f, 0.0f, 0.0f);
}

void OrthoViewPlaneGL::addData(
    const blitz::Array<blitz::TinyVector<float,3>,2>& data) 
{
  if (_data.extent(0) == 0 || _data.extent(1) == 0) 
  {
    _data.resize(data.shape());
    _data = blitz::TinyVector<float,3>(0.0f, 0.0f, 0.0f);
  }
  setFixedSize(
      static_cast<int>(_data.extent(1) * elSizeZoom(1) * zoom()),
      static_cast<int>(_data.extent(0) * elSizeZoom(0) * zoom()));
  _data += data;
}

double OrthoViewPlaneGL::elSizeZoom(int dim) const
{
  return p_orthoView->model()->elementSize()(_dimensions(dim)) /
      p_orthoView->model()->elementSize()(2);
}

double OrthoViewPlaneGL::zoom() const
{
  return p_orthoView->zoom();
}

ptrdiff_t OrthoViewPlaneGL::pos(int dim) const
{
  return p_orthoView->position()(_dimensions(dim));
}

void OrthoViewPlaneGL::initializeGL() 
{
  glClearColor( 0.0, 0.0, 0.0, 0.0 );
  glGenTextures(1, &_texture);
  glBindTexture(GL_TEXTURE_2D, _texture);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glBindTexture(GL_TEXTURE_2D, _texture);
}

void OrthoViewPlaneGL::resizeGL(int w, int h) 
{
  glViewport(0, 0, static_cast<GLint>(w), static_cast<GLint>(h));
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, w + 1, 0, h + 1);
  glMatrixMode(GL_MODELVIEW);
}

void OrthoViewPlaneGL::paintGL() 
{
  glLoadIdentity();
  glScalef(zoom() * elSizeZoom(1), zoom() * elSizeZoom(0), zoom());
  glTranslatef(0.0f, static_cast<GLfloat>(_data.extent(0)), 0.0f);

  glClear(GL_COLOR_BUFFER_BIT);

// Remove due to error in intel video driver in hardy... drm_drawPixels
// throws a segfault, don't ask me why, there seem to be even more severe
// errors, hopefully it runs at all
//   glPixelZoom(elSizeZoom(1) * zoom(), -elSizeZoom(0) * zoom());
//   glRasterPos2i(0, 0);
//   glDrawPixels(static_cast<GLsizei>(_data.extent(1)),
//                static_cast<GLsizei>(_data.extent(0)), GL_RGB,
//                GL_FLOAT, _data.data());

  glTexImage2D(GL_TEXTURE_2D, 0, 3, _data.extent(1), _data.extent(0), 0,
               GL_RGB, GL_FLOAT, _data.data());
  glEnable(GL_TEXTURE_2D);
  glBegin(GL_QUADS);
  glTexCoord2d(0.0, 0.0);
  glVertex2i(0, 0);
  glTexCoord2d(1.0, 0.0);
  glVertex2i(_data.extent(1), 0);
  glTexCoord2d(1.0, 1.0);
  glVertex2i(_data.extent(1), -_data.extent(0));
  glTexCoord2d(0.0, 1.0);
  glVertex2i(0, -_data.extent(0));
  glEnd();
  glDisable(GL_TEXTURE_2D);

  glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

  glBegin(GL_LINES);
  glVertex2i(0, -static_cast<GLint>(pos(0)));
  glVertex2i(static_cast<GLint>(_data.extent(1)), -static_cast<GLint>(pos(0)));
  glVertex2i(static_cast<GLint>(pos(1)), 0);
  glVertex2i(static_cast<GLint>(pos(1)), -static_cast<GLint>(_data.extent(0)));
  glEnd();

  glFlush();
}

void OrthoViewPlaneGL::mouseMoveEvent(QMouseEvent* e) 
{
  blitz::TinyVector<ptrdiff_t,2> scaledPos(
      e->y() / elSizeZoom(0) / zoom(), e->x() / elSizeZoom(1) / zoom());

  // Check whether the user wants to drag a line or the cross
  // If so, set the isDragging flag to Switch from Selection in
  // dragging mode
  blitz::TinyVector<ptrdiff_t,3> pNew = p_orthoView->position();
  if (_dragHorizontal || _dragVertical) 
  {
    if (_dragHorizontal && !_dragVertical) emit vLineDragged(scaledPos(1));
    if (_dragVertical && !_dragHorizontal) emit hLineDragged(scaledPos(0));
    if (_dragHorizontal && _dragVertical)
        emit crosshairDragged(scaledPos(1), scaledPos(0));
    _isDragging = true;
  }

  // Setup Cursor shape according to cross (independent of
  // selection/dragging mode)
  if (std::abs(scaledPos(1) - pos(1)) < 3)
  {
    if (std::abs(scaledPos(0) - pos(0)) < 3) 
    {
      setCursor(Qt::SizeAllCursor);
    }
    else setCursor(Qt::SplitHCursor);
  }
  else if (std::abs(scaledPos(0) - pos(0)) < 3)
  {
    setCursor(Qt::SplitVCursor);
  }
  else setCursor(Qt::CrossCursor);

  // Propagate the mouse move for status actualization (independent of
  // selection/dragging mode)
  emit mouseMoved(scaledPos(1), scaledPos(0));
}

void OrthoViewPlaneGL::mousePressEvent(QMouseEvent* e) 
{
  blitz::TinyVector<ptrdiff_t,2> scaledPos(
      e->y() / elSizeZoom(0) / zoom(), e->x() / elSizeZoom(1) / zoom());

  // If the user left clicked on a line of the crosshair, indicate that
  // subsequent moves are drag events
  if (e->button() == Qt::LeftButton) 
  {
    if (std::abs(scaledPos(1) - pos(1)) < 3) _dragHorizontal = true;
    if (std::abs(scaledPos(0) - pos(0)) < 3) _dragVertical = true;
  }
}

void OrthoViewPlaneGL::mouseReleaseEvent(QMouseEvent* e) 
{
  blitz::TinyVector<ptrdiff_t,2> scaledPos(
      e->y() / elSizeZoom(0) / zoom(), e->x() / elSizeZoom(1) / zoom());

  // If in selection mode emit the selection event at the current position
  if (!_isDragging) 
      emit positionSelected(
          scaledPos(1), scaledPos(0), e->button(), e->modifiers());
  
  // If the left mouse button fired the event restore to selection mode
  if (e->button() == Qt::LeftButton) 
  {
    _dragHorizontal = false;
    _dragVertical = false;
    _isDragging = false;
  }
}

void OrthoViewPlaneGL::wheelEvent(QWheelEvent* e) 
{
  if (e->modifiers().testFlag(Qt::ControlModifier)) // Zoom
  {
    // Avoid zooming to zero size
    if (zoom() + static_cast<double>(e->delta()) / 1200.0 > 0.001)
    {
      p_orthoView->setZoom(zoom() + static_cast<double>(e->delta()) / 1200.0);
      p_orthoView->update();
    }
  }
  else emit mouseWheelMoved(e->delta() / 120);
}
