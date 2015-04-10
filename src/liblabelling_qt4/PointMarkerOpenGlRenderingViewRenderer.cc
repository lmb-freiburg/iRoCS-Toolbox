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

#include "PointMarkerOpenGlRenderingViewRenderer.hh"

#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>
#include <QtXml/QXmlStreamWriter>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include "MultiChannelModel.hh"
#include "AnnotationChannelSpecs.hh"

#include "PointMarker.hh"

#include "UserInteractionEvent.hh"

PointMarkerOpenGlRenderingViewRenderer::PointMarkerOpenGlRenderingViewRenderer(
    PointMarker* marker, OpenGlRenderingViewWidget* view)
        : MarkerOpenGlRenderingViewRenderer(marker, view)
{}

PointMarkerOpenGlRenderingViewRenderer::PointMarkerOpenGlRenderingViewRenderer(
    PointMarkerOpenGlRenderingViewRenderer const &renderer)
        : MarkerOpenGlRenderingViewRenderer(renderer)
{}

PointMarkerOpenGlRenderingViewRenderer::
~PointMarkerOpenGlRenderingViewRenderer()
{}
  
MarkerRenderer *PointMarkerOpenGlRenderingViewRenderer::clone() const
{
  return new PointMarkerOpenGlRenderingViewRenderer(*this);
}

void PointMarkerOpenGlRenderingViewRenderer::render(QPainter*) const
{
  if (p_marker->channel() == NULL)
  {
    std::cerr << "PointMarkerOpenGlRenderingViewRenderer::render(): "
              << "ERROR: Marker at position " << p_marker->positionUm()
              << " has no associated channel!" << std::endl;
    return;
  }

  if (_marker.vertices().size() == 0) initMarker();
  
  PointMarker* marker = static_cast<PointMarker*>(p_marker);
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
  
  glVertexPointer(3, GL_FLOAT, 0, &_marker.vertices()[0]);
  glNormalPointer(GL_FLOAT, 0, &_marker.normals()[0]);
  glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_marker.indices().size()),
                 GL_UNSIGNED_INT, &_marker.indices()[0]);
  
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glPopMatrix();  
}
 
void PointMarkerOpenGlRenderingViewRenderer::userInteractionEvent(
    UserInteractionEvent *)
{}

atb::SurfaceGeometry const
&PointMarkerOpenGlRenderingViewRenderer::geometry() const
{
  if (_marker.vertices().size() == 0) initMarker();
  return _marker;
}

void PointMarkerOpenGlRenderingViewRenderer::initMarker()
{
  std::vector<atb::SurfaceGeometry::VertexT> &V = _marker.vertices();
  std::vector<atb::SurfaceGeometry::NormalT> &N = _marker.normals();
  std::vector<atb::SurfaceGeometry::IndexT> &I = _marker.indices();

  V.resize(32);
  N.resize(32);
  I.resize(180);

  V[ 0] = blitz::TinyVector<float,3>(-0.5f, -0.5f, -3.0f);
  V[ 1] = blitz::TinyVector<float,3>( 0.5f, -0.5f, -3.0f);
  V[ 2] = blitz::TinyVector<float,3>( 0.5f,  0.5f, -3.0f);
  V[ 3] = blitz::TinyVector<float,3>(-0.5f,  0.5f, -3.0f);
  V[ 4] = blitz::TinyVector<float,3>(-0.5f, -0.5f, -0.5f);
  V[ 5] = blitz::TinyVector<float,3>( 0.5f, -0.5f, -0.5f);
  V[ 6] = blitz::TinyVector<float,3>( 0.5f,  0.5f, -0.5f);
  V[ 7] = blitz::TinyVector<float,3>(-0.5f,  0.5f, -0.5f);
  V[ 8] = blitz::TinyVector<float,3>(-0.5f, -0.5f,  0.5f);
  V[ 9] = blitz::TinyVector<float,3>( 0.5f, -0.5f,  0.5f);
  V[10] = blitz::TinyVector<float,3>( 0.5f,  0.5f,  0.5f);
  V[11] = blitz::TinyVector<float,3>(-0.5f,  0.5f,  0.5f);
  V[12] = blitz::TinyVector<float,3>(-0.5f, -0.5f,  3.0f);
  V[13] = blitz::TinyVector<float,3>( 0.5f, -0.5f,  3.0f);
  V[14] = blitz::TinyVector<float,3>( 0.5f,  0.5f,  3.0f);
  V[15] = blitz::TinyVector<float,3>(-0.5f,  0.5f,  3.0f);

  V[16] = blitz::TinyVector<float,3>(-0.5f, -3.0f, -0.5f);
  V[17] = blitz::TinyVector<float,3>(-0.5f, -3.0f,  0.5f);
  V[18] = blitz::TinyVector<float,3>( 0.5f, -3.0f,  0.5f);
  V[19] = blitz::TinyVector<float,3>( 0.5f, -3.0f, -0.5f);
  V[20] = blitz::TinyVector<float,3>(-0.5f,  3.0f, -0.5f);
  V[21] = blitz::TinyVector<float,3>(-0.5f,  3.0f,  0.5f);
  V[22] = blitz::TinyVector<float,3>( 0.5f,  3.0f,  0.5f);
  V[23] = blitz::TinyVector<float,3>( 0.5f,  3.0f, -0.5f);

  V[24] = blitz::TinyVector<float,3>(-3.0f, -0.5f, -0.5f);
  V[25] = blitz::TinyVector<float,3>(-3.0f, -0.5f,  0.5f);
  V[26] = blitz::TinyVector<float,3>(-3.0f,  0.5f,  0.5f);
  V[27] = blitz::TinyVector<float,3>(-3.0f,  0.5f, -0.5f);
  V[28] = blitz::TinyVector<float,3>( 3.0f, -0.5f, -0.5f);
  V[29] = blitz::TinyVector<float,3>( 3.0f, -0.5f,  0.5f);
  V[30] = blitz::TinyVector<float,3>( 3.0f,  0.5f,  0.5f);
  V[31] = blitz::TinyVector<float,3>( 3.0f,  0.5f, -0.5f);

  float s = std::sqrt(3.0f);
  N[ 0] = blitz::TinyVector<float,3>(-s, -s, -s);
  N[ 1] = blitz::TinyVector<float,3>( s, -s, -s);
  N[ 2] = blitz::TinyVector<float,3>( s,  s, -s);
  N[ 3] = blitz::TinyVector<float,3>(-s,  s, -s);
  N[ 4] = blitz::TinyVector<float,3>(-s, -s, -s);
  N[ 5] = blitz::TinyVector<float,3>( s, -s, -s);
  N[ 6] = blitz::TinyVector<float,3>( s,  s, -s);
  N[ 7] = blitz::TinyVector<float,3>(-s,  s, -s);
  N[ 8] = blitz::TinyVector<float,3>(-s, -s,  s);
  N[ 9] = blitz::TinyVector<float,3>( s, -s,  s);
  N[10] = blitz::TinyVector<float,3>( s,  s,  s);
  N[11] = blitz::TinyVector<float,3>(-s,  s,  s);
  N[12] = blitz::TinyVector<float,3>(-s, -s,  s);
  N[13] = blitz::TinyVector<float,3>( s, -s,  s);
  N[14] = blitz::TinyVector<float,3>( s,  s,  s);
  N[15] = blitz::TinyVector<float,3>(-s,  s,  s);
  N[16] = blitz::TinyVector<float,3>(-s, -s, -s);
  N[17] = blitz::TinyVector<float,3>(-s, -s,  s);
  N[18] = blitz::TinyVector<float,3>( s, -s,  s);
  N[19] = blitz::TinyVector<float,3>( s, -s, -s);
  N[20] = blitz::TinyVector<float,3>(-s,  s, -s);
  N[21] = blitz::TinyVector<float,3>(-s,  s,  s);
  N[22] = blitz::TinyVector<float,3>( s,  s,  s);
  N[23] = blitz::TinyVector<float,3>( s,  s, -s);
  N[24] = blitz::TinyVector<float,3>(-s, -s, -s);
  N[25] = blitz::TinyVector<float,3>(-s, -s,  s);
  N[26] = blitz::TinyVector<float,3>(-s,  s,  s);
  N[27] = blitz::TinyVector<float,3>(-s,  s, -s);
  N[28] = blitz::TinyVector<float,3>( s, -s, -s);
  N[29] = blitz::TinyVector<float,3>( s, -s,  s);
  N[30] = blitz::TinyVector<float,3>( s,  s,  s);
  N[31] = blitz::TinyVector<float,3>( s,  s, -s);

  unsigned int ind[] = {
      // x-bar
      0,   2,  1,  0,  3,  2,
      0,   1,  5,  0,  5,  4,   1,  2,  6,  1,  6,  5, 
      2,   3,  7,  2,  7,  6,   0,  4,  3,  3,  4,  7,
      8,   9, 13,  8, 13, 12,   9, 10, 14,  9, 14, 13,
      10, 11, 15, 10, 15, 14,  11,  8, 12, 11, 12, 15,
      12, 13, 14, 12, 14, 15,
      
      // y-bar
      16, 18, 17, 16, 19, 18,
      16, 17,  8, 16,  8,  4,  17, 18,  9, 17,  9,  8,
      18, 19,  5, 18,  5,  9,  19, 16,  4, 19,  4,  5,
      7,  11, 21,  7, 21, 20,  11, 10, 22, 11, 22, 21,
      10,  6, 23, 10, 23, 22,   6,  7, 20,  6, 20, 23,
      20, 21, 22, 20, 22, 23,

      // z-bar
      24, 25, 26, 24, 26, 27,
      25,  8, 11, 25, 11, 26,  26, 11,  7, 26,  7, 27,
      27,  7,  4, 27,  4, 24,  24,  4,  8, 24,  8, 25,
      9,  29, 30,  9, 30, 10,  10, 30, 31, 10, 31,  6,
      6,  28,  5,  6, 31, 28,   5, 28, 29,  5, 29,  9,
      28, 30, 29, 28, 31, 30 };

  std::memcpy(&I[0], ind, 180 * sizeof(unsigned int));
}

atb::SurfaceGeometry PointMarkerOpenGlRenderingViewRenderer::_marker;

