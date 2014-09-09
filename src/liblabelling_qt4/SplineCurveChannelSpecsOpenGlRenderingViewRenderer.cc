/**************************************************************************
**       Title: OpenGlRendering-View SplineCurve-Channel Renderer
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    This class is the interface between the different View- and Channel
**    types. For each Channel type and each View type one such interface
**    class needs to be implemented. This is the specific implementation
**    for SplineCurveChannelSpecs <=> OpenGlRenderingView
**
**************************************************************************/

#include "SplineCurveChannelSpecsOpenGlRenderingViewRenderer.hh"

#include <GL/gl.h>

#include "MultiChannelModel.hh"
#include "SplineCurveChannelSpecs.hh"
#include "OpenGlRenderingViewWidget.hh"

#include <libArrayToolbox/ATBLinAlg.hh>

SplineCurveChannelSpecsOpenGlRenderingViewRenderer::
SplineCurveChannelSpecsOpenGlRenderingViewRenderer(
    SplineCurveChannelSpecs* channel, OpenGlRenderingViewWidget* view)
        : ChannelSpecsOpenGlRenderingViewRenderer(channel, view)
{}
  
SplineCurveChannelSpecsOpenGlRenderingViewRenderer::
~SplineCurveChannelSpecsOpenGlRenderingViewRenderer()
{}

void SplineCurveChannelSpecsOpenGlRenderingViewRenderer::render(QPainter*) const
{
  if (p_view == NULL || !p_view->isVisible()) return;
  if (p_channel == NULL || !p_channel->visible()) return;

  OpenGlRenderingViewWidget *view =
      static_cast<OpenGlRenderingViewWidget*>(p_view);
  SplineCurveChannelSpecs* channel =
      static_cast<SplineCurveChannelSpecs*>(p_channel);
  if (channel->spline() == NULL) return;
  MultiChannelModel* model = channel->model();
  
  double curveLengthUm = channel->spline()->curveIntegral(
      channel->spline()->knot(0), channel->spline()->knot(
          channel->spline()->nKnots() - 1));

  if (channel->hasNewData() ||
      static_cast<int>(_curve.vertices().size()) !=
      static_cast<ptrdiff_t>(curveLengthUm / view->latitudeSamplingUm()) *
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

  // Draw spline
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);  
  glVertexPointer(3, GL_FLOAT, 0, &_curve.vertices()[0]);
  glNormalPointer(GL_FLOAT, 0, &_curve.normals()[0]);
  glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_curve.indices().size()),
                 GL_UNSIGNED_INT, &_curve.indices()[0]);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);

  glPopMatrix();
}

atb::SurfaceGeometry const
&SplineCurveChannelSpecsOpenGlRenderingViewRenderer::curveGeometry() const
{
   OpenGlRenderingViewWidget *view =
      static_cast<OpenGlRenderingViewWidget*>(p_view);
  SplineCurveChannelSpecs* channel =
      static_cast<SplineCurveChannelSpecs*>(p_channel);
  if (view == NULL || channel == NULL || channel->spline() == NULL)
      return _curve;
  
  double curveLengthUm = channel->spline()->curveIntegral(
      channel->spline()->knot(0), channel->spline()->knot(
          channel->spline()->nKnots() - 1));

  if (channel->hasNewData() ||
      static_cast<int>(_curve.vertices().size()) !=
      static_cast<ptrdiff_t>(curveLengthUm / view->latitudeSamplingUm()) *
      view->longitudeSampling() + 2) updateCache();

  return _curve;
}

void SplineCurveChannelSpecsOpenGlRenderingViewRenderer::updateCache() const
{
  // Update geometry cache
  std::vector<atb::SurfaceGeometry::VertexT> &C_v = _curve.vertices();
  std::vector<atb::SurfaceGeometry::NormalT> &C_n = _curve.normals();
  std::vector<atb::SurfaceGeometry::IndexT> &C_i = _curve.indices();

  OpenGlRenderingViewWidget *view =
      static_cast<OpenGlRenderingViewWidget*>(p_view);
  SplineCurveChannelSpecs* channel =
      static_cast<SplineCurveChannelSpecs*>(p_channel);
  
  atb::BSpline< blitz::TinyVector<double,3> > const *spline =
      channel->spline();
  if (spline == NULL) return;

  double curveLengthUm = spline->curveIntegral(
      spline->knot(0), spline->knot(spline->nKnots() - 1));
  
  ptrdiff_t nLat = static_cast<ptrdiff_t>(
      curveLengthUm / view->latitudeSamplingUm());
  ptrdiff_t nLon = view->longitudeSampling();

  C_v.resize(nLat * nLon + 2); // + 2 for the cylinder faces
  C_n.resize(nLat * nLon + 2);
  C_i.resize(6 * (nLat - 1) * nLon + 6 * nLon);
  
  for (ptrdiff_t thetaIdx = 0; thetaIdx < nLat; ++thetaIdx)
  {
    double u = static_cast<double>(thetaIdx) / static_cast<double>(nLat - 1) *
        (spline->knot(spline->nKnots() - 1) - spline->knot(0)) +
        spline->knot(0);

    // Compute axis position at theta
    blitz::TinyVector<double,3> f((*spline)(u));

    // Compute tangent vector at theta
    blitz::TinyVector<double,3> df(spline->derivative(u));
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
      C_n[thetaIdx * nLon + phiIdx] = atb::rotate(normal, tangent, phi);
      C_v[thetaIdx * nLon + phiIdx] = f + C_n[thetaIdx * nLon + phiIdx];
    }
  }

  // The end point vertices
  C_v[C_v.size() - 2] = (*spline)(spline->knot(0));
  C_n[C_v.size() - 2] = -spline->derivative(spline->knot(0));
  C_n[C_v.size() - 2] /= static_cast<float>(
      std::sqrt(blitz::dot(C_n[C_v.size() - 2], C_n[C_v.size() - 2])));
  C_v[C_v.size() - 1] = (*spline)(spline->knot(spline->nKnots() - 1));
  C_n[C_v.size() - 1] = spline->derivative(spline->knot(spline->nKnots() - 1));
  C_n[C_v.size() - 1] /= static_cast<float>(
      std::sqrt(blitz::dot(C_n[C_v.size() - 1], C_n[C_v.size() - 1])));

  // The interior triangles
  for (ptrdiff_t thetaIdx = 0; thetaIdx < nLat - 1; ++thetaIdx)
  {
    for (ptrdiff_t phiIdx = 0; phiIdx < nLon; ++phiIdx)
    {
      C_i[6 * (thetaIdx * nLon + phiIdx)] =
          static_cast<unsigned int>(thetaIdx * nLon + phiIdx);
      C_i[6 * (thetaIdx * nLon + phiIdx) + 1] =
          static_cast<unsigned int>(
              (thetaIdx + 1) * nLon + ((phiIdx + 1) % nLon));
      C_i[6 * (thetaIdx * nLon + phiIdx) + 2] =        
          static_cast<unsigned int>((thetaIdx + 1) * nLon + phiIdx);
      C_i[6 * (thetaIdx * nLon + phiIdx) + 3] =
          static_cast<unsigned int>(thetaIdx * nLon + phiIdx);
      C_i[6 * (thetaIdx * nLon + phiIdx) + 4] =
          static_cast<unsigned int>(
              thetaIdx * nLon + ((phiIdx + 1) % nLon));
      C_i[6 * (thetaIdx * nLon + phiIdx) + 5] =
          static_cast<unsigned int>(
              (thetaIdx + 1) * nLon + ((phiIdx + 1) % nLon));
    }
  }
    
  // Additional triangles for the end faces
  for (ptrdiff_t phiIdx = 0; phiIdx < nLon; ++phiIdx)
  {
    C_i[6 * (nLat - 1) * nLon + 3 * phiIdx] =
        static_cast<unsigned int>(C_v.size() - 2);
    C_i[6 * (nLat - 1) * nLon + 3 * phiIdx + 1] =
        static_cast<unsigned int>((phiIdx + 1) % nLon);
    C_i[6 * (nLat - 1) * nLon + 3 * phiIdx + 2] =
        static_cast<unsigned int>(phiIdx);
    C_i[6 * (nLat - 1) * nLon + 3 * nLon + 3 * phiIdx] =
        static_cast<unsigned int>(C_v.size() - 1);
    C_i[6 * (nLat - 1) * nLon + 3 * nLon + 3 * phiIdx + 1] =
        static_cast<unsigned int>((nLat - 1) * nLon + phiIdx);
    C_i[6 * (nLat - 1) * nLon + 3 * nLon + 3 * phiIdx + 2] =
        static_cast<unsigned int>((nLat - 1) * nLon + (phiIdx + 1) % nLon);
  }
}
