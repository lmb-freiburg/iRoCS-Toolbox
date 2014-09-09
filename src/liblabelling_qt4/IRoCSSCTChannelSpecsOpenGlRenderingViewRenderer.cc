/**************************************************************************
 **       Title: OpenGlRendering-View IRoCS-Channel Renderer for elliptic
 **              root profiles
 **    $RCSfile$
 **   $Revision: 3880 $ $Name$
 **       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
 **   Copyright: GPL $Author: tschmidt $
 ** Description:
 **
 **    This class is the interface between the different View- and Channel
 **    types. For each Channel type and each View type one such interface
 **    class needs to be implemented. This is the specific implementation
 **    for IRoCSSCTChannelSpecs <=> OpenGlRenderingView
 **
 **************************************************************************/

#include "IRoCSSCTChannelSpecsOpenGlRenderingViewRenderer.hh"

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include "MultiChannelModel.hh"
#include "IRoCSSCTChannelSpecs.hh"
#include "OpenGlRenderingViewWidget.hh"

#include <libArrayToolbox/ATBLinAlg.hh>

IRoCSSCTChannelSpecsOpenGlRenderingViewRenderer::
IRoCSSCTChannelSpecsOpenGlRenderingViewRenderer(
    IRoCSSCTChannelSpecs* channel, OpenGlRenderingViewWidget* view)
        : ChannelSpecsOpenGlRenderingViewRenderer(channel, view),
          _axis()
{}
  
IRoCSSCTChannelSpecsOpenGlRenderingViewRenderer::
~IRoCSSCTChannelSpecsOpenGlRenderingViewRenderer()
{}

void IRoCSSCTChannelSpecsOpenGlRenderingViewRenderer::render(QPainter*) const
{
  if (p_view == NULL || !p_view->isVisible()) return;
  if (p_channel == NULL || !p_channel->visible()) return;

  OpenGlRenderingViewWidget *view =
      static_cast<OpenGlRenderingViewWidget*>(p_view);
  IRoCSSCTChannelSpecs* channel = static_cast<IRoCSSCTChannelSpecs*>(p_channel);
  MultiChannelModel* model = channel->model();
  
  channel->sct()->setNLatitudes(
      channel->sct()->axisLengthUm() / view->latitudeSamplingUm());
  channel->sct()->setNLongitudes(view->longitudeSampling());

  if (channel->hasNewData() ||
      static_cast<int>(_axis.vertices().size()) !=
      static_cast<ptrdiff_t>(
          channel->sct()->axisLengthUm() / view->latitudeSamplingUm()) *
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
  glVertexPointer(3, GL_FLOAT, 0, &channel->sct()->surface().vertices()[0]);
  glNormalPointer(GL_FLOAT, 0, &channel->sct()->surface().normals()[0]);
  glDrawElements(
      GL_TRIANGLES, static_cast<GLsizei>(
          channel->sct()->surface().indices().size()),
      GL_UNSIGNED_INT, &channel->sct()->surface().indices()[0]);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glPopAttrib();

  glPopMatrix();
}

atb::SurfaceGeometry const
&IRoCSSCTChannelSpecsOpenGlRenderingViewRenderer::axisGeometry() const
{
  OpenGlRenderingViewWidget *view =
      static_cast<OpenGlRenderingViewWidget*>(p_view);
  IRoCSSCTChannelSpecs* channel = static_cast<IRoCSSCTChannelSpecs*>(p_channel);
  if (view == NULL || channel == NULL) return _axis;
  
  if (channel->hasNewData() ||
      static_cast<int>(_axis.vertices().size()) !=
      static_cast<ptrdiff_t>(
          channel->sct()->axisLengthUm() / view->latitudeSamplingUm()) *
      view->longitudeSampling() + 2) updateCache();

  return _axis;
}

void IRoCSSCTChannelSpecsOpenGlRenderingViewRenderer::updateCache() const
{
  // Update geometry cache
  std::vector<atb::SurfaceGeometry::VertexT> &V_a = _axis.vertices();
  std::vector<atb::SurfaceGeometry::NormalT> &N_a = _axis.normals();
  std::vector<atb::SurfaceGeometry::IndexT> &I_a = _axis.indices();

  OpenGlRenderingViewWidget *view =
      static_cast<OpenGlRenderingViewWidget*>(p_view);
  IRoCSSCTChannelSpecs* channel = static_cast<IRoCSSCTChannelSpecs*>(p_channel);
  if (view == NULL || channel == NULL) return;

  ShellCoordinateTransform *sct = channel->sct();
  if (sct == NULL) return;
  
  ptrdiff_t nLat = static_cast<ptrdiff_t>(
      sct->axisLengthUm() / view->latitudeSamplingUm());
  ptrdiff_t nLon = view->longitudeSampling();

  V_a.resize(nLat * nLon + 2); // + 2 for the cylinder faces
  N_a.resize(nLat * nLon + 2);
  I_a.resize(6 * (nLat - 1) * nLon + 6 * nLon);
  
  for (ptrdiff_t thetaIdx = 0; thetaIdx < nLat; ++thetaIdx)
  {
    double u = static_cast<double>(thetaIdx) / static_cast<double>(nLat - 1);

    // Compute axis position at theta
    blitz::TinyVector<double,3> f(sct->getAxisPosition(u));

    // Compute tangent vector at theta
    blitz::TinyVector<double,3> df(sct->getAxisDirection(u));
    double dfNorm = std::sqrt(blitz::dot(df, df));
    blitz::TinyVector<double,3> tangent(df / dfNorm);

    // Generate orthogonal circle
    blitz::TinyVector<double,3> normal(1.0, 0.0, 0.0);
    normal -= blitz::dot(normal, tangent) * tangent;
    normal /= std::sqrt(blitz::dot(normal, normal));
    for (ptrdiff_t phiIdx = 0; phiIdx < nLon; ++phiIdx)
    {
      double phi = static_cast<double>(phiIdx) /
          static_cast<double>(nLon) * 2.0 * M_PI;
      N_a[thetaIdx * nLon + phiIdx] = atb::rotate(normal, tangent, phi);
      V_a[thetaIdx * nLon + phiIdx] = f + N_a[thetaIdx * nLon + phiIdx];
    }
  }

  // The end point vertices
  V_a[V_a.size() - 2] = sct->getAxisPosition(0.0);
  N_a[V_a.size() - 2] = -sct->getAxisDirection(0.0);
  N_a[V_a.size() - 2] /= static_cast<float>(
      std::sqrt(blitz::dot(N_a[V_a.size() - 2], N_a[V_a.size() - 2])));
  V_a[V_a.size() - 1] = sct->getAxisPosition(1.0);
  N_a[V_a.size() - 1] = sct->getAxisDirection(1.0);
  N_a[V_a.size() - 1] /= static_cast<float>(
      std::sqrt(blitz::dot(N_a[V_a.size() - 1], N_a[V_a.size() - 1])));

  // The interior triangles
  for (ptrdiff_t thetaIdx = 0; thetaIdx < nLat - 1; ++thetaIdx)
  {
    for (ptrdiff_t phiIdx = 0; phiIdx < nLon; ++phiIdx)
    {
      I_a[6 * (thetaIdx * nLon + phiIdx)] =
          static_cast<unsigned int>(thetaIdx * nLon + phiIdx);
      I_a[6 * (thetaIdx * nLon + phiIdx) + 1] =
          static_cast<unsigned int>(
              (thetaIdx + 1) * nLon + ((phiIdx + 1) % nLon));
      I_a[6 * (thetaIdx * nLon + phiIdx) + 2] =        
          static_cast<unsigned int>((thetaIdx + 1) * nLon + phiIdx);
      I_a[6 * (thetaIdx * nLon + phiIdx) + 3] =
          static_cast<unsigned int>(thetaIdx * nLon + phiIdx);
      I_a[6 * (thetaIdx * nLon + phiIdx) + 4] =
          static_cast<unsigned int>(
              thetaIdx * nLon + ((phiIdx + 1) % nLon));
      I_a[6 * (thetaIdx * nLon + phiIdx) + 5] =
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
}
