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

#include "SurfaceMarkerOpenGlRenderingViewRenderer.hh"

#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>
#include <QtXml/QXmlStreamWriter>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include "MultiChannelModel.hh"
#include "AnnotationChannelSpecs.hh"

#include "SurfaceMarker.hh"
#include "OpenGlRenderingViewWidget.hh"

#include "UserInteractionEvent.hh"

SurfaceMarkerOpenGlRenderingViewRenderer::
SurfaceMarkerOpenGlRenderingViewRenderer(
    SurfaceMarker* marker, OpenGlRenderingViewWidget* view)
        : MarkerOpenGlRenderingViewRenderer(marker, view)
{}

SurfaceMarkerOpenGlRenderingViewRenderer::
SurfaceMarkerOpenGlRenderingViewRenderer(
    SurfaceMarkerOpenGlRenderingViewRenderer const &renderer)
        : MarkerOpenGlRenderingViewRenderer(renderer)
{}

SurfaceMarkerOpenGlRenderingViewRenderer::
~SurfaceMarkerOpenGlRenderingViewRenderer()
{}
  
MarkerRenderer *SurfaceMarkerOpenGlRenderingViewRenderer::clone() const
{
  return new SurfaceMarkerOpenGlRenderingViewRenderer(*this);
}

void SurfaceMarkerOpenGlRenderingViewRenderer::render(QPainter*) const
{
  if (p_marker->channel() == NULL)
  {
    std::cerr << "SurfaceMarkerOpenGlRenderingViewRenderer::render(): "
              << "ERROR: Marker at position " << p_marker->positionUm()
              << " has no asociated channel!" << std::endl;
    return;
  }

  SurfaceMarker* marker = reinterpret_cast<SurfaceMarker*>(p_marker);
  AnnotationChannelSpecs* channel = marker->channel();
  MultiChannelModel* model = channel->model();

  blitz::TinyVector<float,3> color(channel->colorForLabel(marker->label()));
  float alpha = channel->alpha() * channel->alphaForLabel(marker->label());
  
  if (alpha == 0.0f) return;

  _setPenColor(marker->selected());

  glPushMatrix();
  glTranslated(
      marker->positionUm()(0) -
      (model->lowerBoundUm()(0) + model->upperBoundUm()(0)) / 2.0f,
      marker->positionUm()(1) -
      (model->lowerBoundUm()(1) + model->upperBoundUm()(1)) / 2.0f,
      marker->positionUm()(2) -
      (model->lowerBoundUm()(2) + model->upperBoundUm()(2)) / 2.0f);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  
  glVertexPointer(3, GL_FLOAT, 0, &marker->vertices()[0]);
  glNormalPointer(GL_FLOAT, 0, &marker->normals()[0]);
  glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(marker->indices().size()),
                 GL_UNSIGNED_INT, &marker->indices()[0]);
  
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glPopMatrix();
}

void SurfaceMarkerOpenGlRenderingViewRenderer::userInteractionEvent(
    UserInteractionEvent *)
{}
