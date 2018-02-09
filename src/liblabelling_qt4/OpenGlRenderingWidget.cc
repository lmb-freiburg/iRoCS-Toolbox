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

#include "OpenGlRenderingWidget.hh"

#include "OpenGlRenderingViewWidget.hh"
#include "OpenGlRenderingSettingsWidget.hh"
#include "MultiChannelModel.hh"
#include "ChannelSpecsRenderer.hh"

#include <GL/glu.h>

OpenGlRenderingWidget::OpenGlRenderingWidget(
    OpenGlRenderingViewWidget *view, QWidget *parent)
        : QGLWidget(parent), p_view(view), _lastMousePosition(QPoint(-1, -1)),
          _distanceToOrigin(100.0f)
{
  _rotation =
      1.0, 0.0, 0.0, 0.0,
      0.0, 1.0, 0.0, 0.0,
      0.0, 0.0, 1.0, 0.0,
      0.0, 0.0, 0.0, 1.0;

  _translation = 0.0f, 0.0f;

  setMouseTracking(true);
}

OpenGlRenderingWidget::~OpenGlRenderingWidget()
{}

void OpenGlRenderingWidget::initializeGL()
{
  blitz::TinyVector<float,3> bgCol(
      p_view->p_renderingSettings->backgroundColor() / 255.0f);
  glClearColor(bgCol(0), bgCol(1), bgCol(2), 0.0);
  glCullFace(GL_BACK);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_NORMALIZE);
  if (p_view->p_renderingSettings->lightEnabled()) glEnable(GL_LIGHTING);
  else glDisable(GL_LIGHTING);
  if (p_view->p_renderingSettings->backFaceCullingEnabled())
      glEnable(GL_CULL_FACE);
  else glDisable(GL_CULL_FACE);
  switch (p_view->p_renderingSettings->frontFaceRendering())
  {
  case OpenGlRenderingSettingsWidget::Solid:
    glPolygonMode(GL_FRONT, GL_FILL);
    break;
  case OpenGlRenderingSettingsWidget::Wireframe:
    glPolygonMode(GL_FRONT, GL_LINE);
    break;
  case OpenGlRenderingSettingsWidget::Dots:
    glPolygonMode(GL_FRONT, GL_POINT);
    break;
  default:
    std::cerr << __FILE__ << ":" << __LINE__ << ": Missing implementation"
              << std::endl;
    exit(-1);
  }
  switch (p_view->p_renderingSettings->backFaceRendering())
  {
  case OpenGlRenderingSettingsWidget::Solid:
    glPolygonMode(GL_BACK, GL_FILL);
    break;
  case OpenGlRenderingSettingsWidget::Wireframe:
    glPolygonMode(GL_BACK, GL_LINE);
    break;
  case OpenGlRenderingSettingsWidget::Dots:
    glPolygonMode(GL_BACK, GL_POINT);
    break;
  default:
    std::cerr << __FILE__ << ":" << __LINE__ << ": Missing implementation"
              << std::endl;
    exit(-1);
  }

  float shininess[] = { p_view->p_renderingSettings->materialShininess() };
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}

void OpenGlRenderingWidget::resizeGL(int width, int height)
{
  glViewport(0, 0, static_cast<GLint>(width), static_cast<GLint>(height));
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(
      35.0, static_cast<GLdouble>(width) / static_cast<GLdouble>(height),
      1.0, 5000.0);
}

void OpenGlRenderingWidget::paintGL()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  float light0Position[] = {1.0, 1.0, 1.0, 0.0};
  float light0Color[] = {1.0, 1.0, 1.0, 1.0};
  glLightfv(GL_LIGHT0, GL_POSITION, light0Position);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light0Color);
  glEnable(GL_LIGHT0);

  glTranslatef(_translation(0), _translation(1), -_distanceToOrigin);
  glMultMatrixd(_rotation.data());

  if (p_view == NULL || p_view->p_model == NULL) return;

  for (std::vector<ChannelSpecs*>::iterator it = p_view->p_model->begin();
       it != p_view->p_model->end(); ++it)
  {
    if (!(*it)->visible()) continue;
    if ((*it)->renderer(p_view) == NULL)
    {
      std::cerr << __FILE__ << ":" << __LINE__ << ": No associated renderer "
                << "for channel " << (*it)->name() << " and view " << this
                << " found - Please report this bug" << std::endl;
      exit(-1);
    }
    (*it)->renderer(p_view)->render();
  }
}

void OpenGlRenderingWidget::resetView()
{
  _rotation =
      1.0, 0.0, 0.0, 0.0,
      0.0, 1.0, 0.0, 0.0,
      0.0, 0.0, 1.0, 0.0,
      0.0, 0.0, 0.0, 1.0;
  _translation = 0.0f, 0.0f;
  _distanceToOrigin = 100.0f;
  updateGL();
}

void OpenGlRenderingWidget::mousePressEvent(QMouseEvent *e)
{
  if (e->button() == Qt::LeftButton)
  {
    _lastMousePosition = e->pos();
    e->accept();
    return;
  }
}

void OpenGlRenderingWidget::mouseMoveEvent(QMouseEvent *e)
{
  if (e->buttons().testFlag(Qt::LeftButton))
  {
    if (_lastMousePosition.x() != -1 && _lastMousePosition.y() != -1)
    {
      if (e->modifiers().testFlag(Qt::ShiftModifier)) // Move
      {
        blitz::TinyVector<float,2> offset(
            static_cast<float>(e->x() - _lastMousePosition.x()),
            static_cast<float>(e->y() - _lastMousePosition.y()));
        _translation += offset;
      }
      else if (e->modifiers().testFlag(Qt::ControlModifier)) // Rotate roll
      {
        double offset = 10.0 * M_PI *
            static_cast<double>(_lastMousePosition.x() - e->x()) / 180.0;
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixd(_rotation.data());
        glRotated(offset, 0.0, 0.0, 1.0);
        glGetDoublev(GL_MODELVIEW_MATRIX, _rotation.data());
      }
      else // Rotate aximuth and elevation
      {
        blitz::TinyVector<double,2> offset(
            10.0 * M_PI * static_cast<double>(_lastMousePosition.x() - e->x()) /
            180.0,
            10.0 * M_PI * static_cast<double>(_lastMousePosition.y() - e->y()) /
            180.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixd(_rotation.data());
        blitz::TinyVector<double,3> axis(-offset(1), offset(0), 0.0);
        double angle = std::sqrt(blitz::dot(axis, axis));
        if (angle != 0.0)
        {
          axis /= angle;
          glRotated(angle, axis(0), axis(1), axis(2));
          glGetDoublev(GL_MODELVIEW_MATRIX, _rotation.data());
        }
      }
    }
    _lastMousePosition = e->pos();
    updateGL();
    e->accept();
    return;
  }

  _lastMousePosition = QPoint(-1, -1);
}

void OpenGlRenderingWidget::mouseReleaseEvent(QMouseEvent *e)
{
  if (e->button() == Qt::LeftButton)
  {
    _lastMousePosition = QPoint(-1, -1);
    e->accept();
    return;
  }
}

void OpenGlRenderingWidget::wheelEvent(QWheelEvent *e)
{
  if (e->modifiers() == Qt::NoModifier)
  {
    _distanceToOrigin -= static_cast<float>(e->delta());
    updateGL();
    e->accept();
    return;
  }
}
