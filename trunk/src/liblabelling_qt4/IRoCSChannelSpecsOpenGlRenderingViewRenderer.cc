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

#include "IRoCSChannelSpecsOpenGlRenderingViewRenderer.hh"

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include "MultiChannelModel.hh"
#include "IRoCSChannelSpecs.hh"
#include "OpenGlRenderingViewWidget.hh"

#include <libArrayToolbox/ATBLinAlg.hh>

IRoCSChannelSpecsOpenGlRenderingViewRenderer::
IRoCSChannelSpecsOpenGlRenderingViewRenderer(
    IRoCSChannelSpecs* channel, OpenGlRenderingViewWidget* view)
        : ChannelSpecsOpenGlRenderingViewRenderer(channel, view),
          _axis(), _boundary()
{}
  
IRoCSChannelSpecsOpenGlRenderingViewRenderer::
~IRoCSChannelSpecsOpenGlRenderingViewRenderer()
{}

void IRoCSChannelSpecsOpenGlRenderingViewRenderer::render(QPainter*) const
{
  if (p_view == NULL || !p_view->isVisible()) return;
  if (p_channel == NULL || !p_channel->visible()) return;

  OpenGlRenderingViewWidget *view =
      static_cast<OpenGlRenderingViewWidget*>(p_view);
  IRoCSChannelSpecs* channel = static_cast<IRoCSChannelSpecs*>(p_channel);
  MultiChannelModel* model = channel->model();
  
  double axisLengthUm = channel->rct()->axisSpline().curveIntegral(
      channel->rct()->axisSpline().knot(0),
      channel->rct()->axisSpline().knot(
          channel->rct()->axisSpline().nKnots() - 1));

  if (channel->hasNewData() ||
      static_cast<int>(_axis.vertices().size()) !=
      static_cast<ptrdiff_t>(axisLengthUm / view->latitudeSamplingUm()) *
      view->longitudeSampling() + 2) updateCache();

  blitz::TinyVector<float,3> color(channel->color());
  float alpha = channel->alpha();
  
  // Unlit case
  glColor4f(color(0), color(1), color(2), alpha);
  
  // With lighting enabled
  float ambientColor[] =
      { 0.1f * color(0), 0.1f * color(1), 0.1f * color(2), alpha };
  float diffColor[] = { color(0), color(1), color(2), alpha };
  float specColor[] = { 1.0f, 1.0f, 1.0f, alpha };
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientColor);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffColor);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specColor);
  
  // Center the rendering
  glPushMatrix();
  glTranslated(
      -(model->lowerBoundUm()(0) + model->upperBoundUm()(0)) / 2.0f,
      -(model->lowerBoundUm()(1) + model->upperBoundUm()(1)) / 2.0f,
      -(model->lowerBoundUm()(2) + model->upperBoundUm()(2)) / 2.0f);

  // Draw axis
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);  
  glVertexPointer(3, GL_FLOAT, 0, &_axis.vertices()[0]);
  glNormalPointer(GL_FLOAT, 0, &_axis.normals()[0]);
  glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_axis.indices().size()),
                 GL_UNSIGNED_INT, &_axis.indices()[0]);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);

  // Draw boundary surface
  glPushAttrib(GL_POLYGON_BIT);
  glPolygonMode(GL_FRONT, GL_LINE);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);  
  glVertexPointer(3, GL_FLOAT, 0, &_boundary.vertices()[0]);
  glNormalPointer(GL_FLOAT, 0, &_boundary.normals()[0]);
  glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_boundary.indices().size()),
                 GL_UNSIGNED_INT, &_boundary.indices()[0]);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glPopAttrib();

  glPopMatrix();
}

atb::SurfaceGeometry const
&IRoCSChannelSpecsOpenGlRenderingViewRenderer::axisGeometry() const
{
  OpenGlRenderingViewWidget *view =
      static_cast<OpenGlRenderingViewWidget*>(p_view);
  IRoCSChannelSpecs* channel = static_cast<IRoCSChannelSpecs*>(p_channel);
  if (view == NULL || channel == NULL) return _axis;
  
  double axisLengthUm = channel->rct()->axisSpline().curveIntegral(
      channel->rct()->axisSpline().knot(0),
      channel->rct()->axisSpline().knot(
          channel->rct()->axisSpline().nKnots() - 1));

  if (channel->hasNewData() ||
      static_cast<int>(_axis.vertices().size()) !=
      static_cast<ptrdiff_t>(axisLengthUm / view->latitudeSamplingUm()) *
      view->longitudeSampling() + 2) updateCache();

  return _axis;
}

atb::SurfaceGeometry const
&IRoCSChannelSpecsOpenGlRenderingViewRenderer::boundaryGeometry() const
{
  OpenGlRenderingViewWidget *view =
      static_cast<OpenGlRenderingViewWidget*>(p_view);
  IRoCSChannelSpecs* channel = static_cast<IRoCSChannelSpecs*>(p_channel);
  if (view == NULL || channel == NULL) return _boundary;
  
  double axisLengthUm = channel->rct()->axisSpline().curveIntegral(
      channel->rct()->axisSpline().knot(0),
      channel->rct()->axisSpline().knot(
          channel->rct()->axisSpline().nKnots() - 1));

  if (channel->hasNewData() ||
      static_cast<int>(_axis.vertices().size()) !=
      static_cast<ptrdiff_t>(axisLengthUm / view->latitudeSamplingUm()) *
      view->longitudeSampling() + 2) updateCache();

  return _boundary;
}

void IRoCSChannelSpecsOpenGlRenderingViewRenderer::updateCache() const
{
  // Update geometry cache
  std::vector<atb::SurfaceGeometry::VertexT> &V_a = _axis.vertices();
  std::vector<atb::SurfaceGeometry::VertexT> &V_b = _boundary.vertices();
  std::vector<atb::SurfaceGeometry::NormalT> &N_a = _axis.normals();
  std::vector<atb::SurfaceGeometry::NormalT> &N_b = _boundary.normals();
  std::vector<atb::SurfaceGeometry::IndexT> &I_a = _axis.indices();
  std::vector<atb::SurfaceGeometry::IndexT> &I_b = _boundary.indices();

  OpenGlRenderingViewWidget *view =
      static_cast<OpenGlRenderingViewWidget*>(p_view);
  IRoCSChannelSpecs* channel = static_cast<IRoCSChannelSpecs*>(p_channel);
  if (view == NULL || channel == NULL) return;

  atb::IRoCS *rct = channel->rct();
  if (rct == NULL) return;
  
  atb::BSpline< blitz::TinyVector<double,3> > const &axisSpline =
      rct->axisSpline();
  atb::BSpline<double> const &thicknessSpline = rct->thicknessSpline();
  double axisLengthUm = axisSpline.curveIntegral(
      axisSpline.knot(0), axisSpline.knot(axisSpline.nKnots() - 1));
  
  ptrdiff_t nLat = static_cast<ptrdiff_t>(
      axisLengthUm / view->latitudeSamplingUm());
  ptrdiff_t nLon = view->longitudeSampling();

  V_a.resize(nLat * nLon + 2); // + 2 for the cylinder faces
  N_a.resize(nLat * nLon + 2);
  I_a.resize(6 * (nLat - 1) * nLon + 6 * nLon);
  V_b.resize(nLat * nLon);
  N_b.resize(nLat * nLon, blitz::TinyVector<float,3>(0.0f));
  I_b.resize(6 * (nLat - 1) * nLon);
  
  for (ptrdiff_t thetaIdx = 0; thetaIdx < nLat; ++thetaIdx)
  {
    double u = static_cast<double>(thetaIdx) / static_cast<double>(nLat - 1) *
        (axisSpline.knot(axisSpline.nKnots() - 1) - axisSpline.knot(0)) +
        axisSpline.knot(0);

    // Compute axis position at theta
    blitz::TinyVector<double,3> f(axisSpline(u));

    // Compute tangent vector at theta
    blitz::TinyVector<double,3> df(axisSpline.derivative(u));
    double dfNorm = std::sqrt(blitz::dot(df, df));
    blitz::TinyVector<double,3> tangent(df / dfNorm);

    // Generate orthogonal circle
    blitz::TinyVector<double,3> normal(1.0, 0.0, 0.0);
    normal -= blitz::dot(normal, tangent) * tangent;
    normal /= std::sqrt(blitz::dot(normal, normal));
    double t = thicknessSpline(u);
    for (ptrdiff_t phiIdx = 0; phiIdx < nLon; ++phiIdx)
    {
      double phi = static_cast<double>(phiIdx) /
          static_cast<double>(nLon) * 2.0 * M_PI;
      N_a[thetaIdx * nLon + phiIdx] = atb::rotate(normal, tangent, phi);
      V_a[thetaIdx * nLon + phiIdx] = f + N_a[thetaIdx * nLon + phiIdx];
      V_b[thetaIdx * nLon + phiIdx] = f + N_a[thetaIdx * nLon + phiIdx] * t;
    }
  }

  // The end point vertices
  V_a[V_a.size() - 2] = axisSpline(axisSpline.knot(0));
  N_a[V_a.size() - 2] = -axisSpline.derivative(axisSpline.knot(0));
  N_a[V_a.size() - 2] /= static_cast<float>(
      std::sqrt(blitz::dot(N_a[V_a.size() - 2], N_a[V_a.size() - 2])));
  V_a[V_a.size() - 1] = axisSpline(axisSpline.knot(axisSpline.nKnots() - 1));
  N_a[V_a.size() - 1] = axisSpline.derivative(
      axisSpline.knot(axisSpline.nKnots() - 1));
  N_a[V_a.size() - 1] /= static_cast<float>(
      std::sqrt(blitz::dot(N_a[V_a.size() - 1], N_a[V_a.size() - 1])));

  // The interior triangles
  for (ptrdiff_t thetaIdx = 0; thetaIdx < nLat - 1; ++thetaIdx)
  {
    for (ptrdiff_t phiIdx = 0; phiIdx < nLon; ++phiIdx)
    {
      I_a[6 * (thetaIdx * nLon + phiIdx)] =
          I_b[6 * (thetaIdx * nLon + phiIdx)] =
          static_cast<unsigned int>(thetaIdx * nLon + phiIdx);
      I_a[6 * (thetaIdx * nLon + phiIdx) + 1] =
          I_b[6 * (thetaIdx * nLon + phiIdx) + 1] =
          static_cast<unsigned int>(
              (thetaIdx + 1) * nLon + ((phiIdx + 1) % nLon));
      I_a[6 * (thetaIdx * nLon + phiIdx) + 2] =        
          I_b[6 * (thetaIdx * nLon + phiIdx) + 2] =        
          static_cast<unsigned int>((thetaIdx + 1) * nLon + phiIdx);
      I_a[6 * (thetaIdx * nLon + phiIdx) + 3] =
          I_b[6 * (thetaIdx * nLon + phiIdx) + 3] =
          static_cast<unsigned int>(thetaIdx * nLon + phiIdx);
      I_a[6 * (thetaIdx * nLon + phiIdx) + 4] =
          I_b[6 * (thetaIdx * nLon + phiIdx) + 4] =
          static_cast<unsigned int>(
              thetaIdx * nLon + ((phiIdx + 1) % nLon));
      I_a[6 * (thetaIdx * nLon + phiIdx) + 5] =
          I_b[6 * (thetaIdx * nLon + phiIdx) + 5] =
          static_cast<unsigned int>(
              (thetaIdx + 1) * nLon + ((phiIdx + 1) % nLon));
    }
  }
    
  // Additional triangles for the axis end faces
  for (ptrdiff_t phiIdx = 0; phiIdx < nLon; ++phiIdx)
  {
    I_a[6 * (nLat - 1) * nLon + 3 * phiIdx] =
        static_cast<unsigned int>(V_a.size() - 2);
    I_a[6 * (nLat - 1) * nLon + 3 * phiIdx + 1] =
        static_cast<unsigned int>((phiIdx + 1) % nLon);
    I_a[6 * (nLat - 1) * nLon + 3 * phiIdx + 2] =
        static_cast<unsigned int>(phiIdx);
    I_a[6 * (nLat - 1) * nLon + 3 * nLon + 3 * phiIdx] =
        static_cast<unsigned int>(V_a.size() - 1);
    I_a[6 * (nLat - 1) * nLon + 3 * nLon + 3 * phiIdx + 1] =
        static_cast<unsigned int>((nLat - 1) * nLon + phiIdx);
    I_a[6 * (nLat - 1) * nLon + 3 * nLon + 3 * phiIdx + 2] =
        static_cast<unsigned int>((nLat - 1) * nLon + (phiIdx + 1) % nLon);
  }

  // Compute boundary surface normals
  for (size_t i = 0; i < I_b.size(); i += 3)
  {
    for (int d = 0; d < 3; ++d)
    {
      blitz::TinyVector<float,3> normal(
          blitz::cross(
              blitz::TinyVector<float,3>(
                  V_b[I_b[i + (d + 1) % 3]] - V_b[I_b[i + d]]),
              blitz::TinyVector<float,3>(
                  V_b[I_b[i + (d + 2) % 3]] - V_b[I_b[i + d]])));
      float n = static_cast<float>(std::sqrt(blitz::dot(normal, normal)));
      if (n > 0.0f) N_b[I_b[i + d]] += normal / n;
    }
  }
    
  // And normalize them
  for (size_t i = 0; i < N_b.size(); ++i)
  {
    float n = static_cast<float>(std::sqrt(blitz::dot(N_b[i], N_b[i])));
    if (n > 0.0f) N_b[i] /= n;
    else N_b[i] = blitz::TinyVector<float,3>(1.0f, 0.0f, 0.0f);
  }    
}
