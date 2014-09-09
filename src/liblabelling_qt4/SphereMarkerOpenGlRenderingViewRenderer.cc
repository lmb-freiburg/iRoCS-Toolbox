/**************************************************************************
**       Title: SphereMarker-OpenGlRenderingView Renderer
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    This class is the interface between the different View- and Marker
**    types. For each Marker type and each View type one such interface
**    class needs to be implemented. This is the specific implementation
**    for SphereMarker <=> OpenGlRenderingView
**
**************************************************************************/

#include "SphereMarkerOpenGlRenderingViewRenderer.hh"

#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>
#include <QtXml/QXmlStreamWriter>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include "MultiChannelModel.hh"
#include "AnnotationChannelSpecs.hh"

#include "SphereMarker.hh"

#include "UserInteractionEvent.hh"

SphereMarkerOpenGlRenderingViewRenderer::
SphereMarkerOpenGlRenderingViewRenderer(
    SphereMarker* marker, OpenGlRenderingViewWidget* view)
        : MarkerOpenGlRenderingViewRenderer(marker, view)
{}

SphereMarkerOpenGlRenderingViewRenderer::
SphereMarkerOpenGlRenderingViewRenderer(
    SphereMarkerOpenGlRenderingViewRenderer const &renderer)
        : MarkerOpenGlRenderingViewRenderer(renderer)
{}

SphereMarkerOpenGlRenderingViewRenderer::
~SphereMarkerOpenGlRenderingViewRenderer()
{}
  
MarkerRenderer *SphereMarkerOpenGlRenderingViewRenderer::clone() const
{
  return new SphereMarkerOpenGlRenderingViewRenderer(*this);
}

void SphereMarkerOpenGlRenderingViewRenderer::render(QPainter*) const
{
  if (p_marker->channel() == NULL)
  {
    std::cerr << "PointMarkerOpenGlRenderingViewRenderer::render(): "
              << "ERROR: Marker at position " << p_marker->positionUm()
              << " has no asociated channel!" << std::endl;
    return;
  }

  if (_marker.vertices().size() == 0) initMarker();
  
  SphereMarker* marker = static_cast<SphereMarker*>(p_marker);
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
  glScaled(marker->radiusUm(), marker->radiusUm(), marker->radiusUm());

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

void SphereMarkerOpenGlRenderingViewRenderer::userInteractionEvent(
    UserInteractionEvent *)
{}

atb::SurfaceGeometry const
&SphereMarkerOpenGlRenderingViewRenderer::geometry() const
{
  if (_marker.vertices().size() == 0) initMarker();
  return _marker;
}

void SphereMarkerOpenGlRenderingViewRenderer::initMarker()
{
  size_t nLat = 10, nLon = 20;

  std::vector<atb::SurfaceGeometry::VertexT> &V = _marker.vertices();
  std::vector<atb::SurfaceGeometry::NormalT> &N = _marker.normals();
  std::vector<atb::SurfaceGeometry::IndexT> &I = _marker.indices();

  V.resize(nLat * nLon + 2);
  N.resize(nLat * nLon + 2);
  I.resize(6 * (nLat - 1) * nLon + 6 * nLon);

  for (size_t thetaIdx = 0; thetaIdx < nLat; ++thetaIdx)
  {
    double theta = static_cast<double>(thetaIdx + 1) /
        static_cast<double>(nLat + 1) * M_PI;
    for (size_t phiIdx = 0; phiIdx < nLon; ++phiIdx)
    {
      double phi = static_cast<double>(phiIdx) / static_cast<double>(nLon) *
          2.0 * M_PI;
      V[thetaIdx * nLon + phiIdx] =
          static_cast<float>(std::cos(theta)),
          static_cast<float>(std::cos(phi) * std::sin(theta)),
          static_cast<float>(std::sin(phi) * std::sin(theta));
      N[thetaIdx * nLon + phiIdx] =
          static_cast<float>(std::cos(theta)),
          static_cast<float>(std::cos(phi) * std::sin(theta)),
          static_cast<float>(std::sin(phi) * std::sin(theta));
    }
  }

  // The pole vertices
  V[nLat * nLon] = 1.0f, 0.0f, 0.0f;
  N[nLat * nLon] = 1.0f, 0.0f, 0.0f;
  V[nLat * nLon + 1] = -1.0f, 0.0f, 0.0f;
  N[nLat * nLon + 1] = -1.0f, 0.0f, 0.0f;

  // And the indices
  for (ptrdiff_t thetaIdx = 0; thetaIdx < static_cast<ptrdiff_t>(nLat) - 1;
       ++thetaIdx)
  {
    for (ptrdiff_t phiIdx = 0; phiIdx < static_cast<ptrdiff_t>(nLon); ++phiIdx)
    {
      I[6 * (thetaIdx * nLon + phiIdx)] =
          static_cast<unsigned int>(thetaIdx * nLon + phiIdx);
      I[6 * (thetaIdx * nLon + phiIdx) + 1] =
          static_cast<unsigned int>((thetaIdx + 1) * nLon + phiIdx);
      I[6 * (thetaIdx * nLon + phiIdx) + 2] =        
          static_cast<unsigned int>(
              (thetaIdx + 1) * nLon + ((phiIdx + 1) % nLon));
      I[6 * (thetaIdx * nLon + phiIdx) + 3] =
          static_cast<unsigned int>(thetaIdx * nLon + phiIdx);
      I[6 * (thetaIdx * nLon + phiIdx) + 4] =
          static_cast<unsigned int>(
              (thetaIdx + 1) * nLon + ((phiIdx + 1) % nLon));
      I[6 * (thetaIdx * nLon + phiIdx) + 5] =
          static_cast<unsigned int>(
              thetaIdx * nLon + ((phiIdx + 1) % nLon));
    }
  }

  // Pole triangles
  for (ptrdiff_t phiIdx = 0; phiIdx < static_cast<ptrdiff_t>(nLon); ++phiIdx)
  {
    I[6 * (nLat - 1) * nLon + 3 * phiIdx] =
        static_cast<unsigned int>(V.size() - 2);
    I[6 * (nLat - 1) * nLon + 3 * phiIdx + 1] =
        static_cast<unsigned int>(phiIdx);
    I[6 * (nLat - 1) * nLon + 3 * phiIdx + 2] =
        static_cast<unsigned int>((phiIdx + 1) % nLon);
    I[6 * (nLat - 1) * nLon + 3 * nLon + 3 * phiIdx] =
        static_cast<unsigned int>(V.size() - 1);
    I[6 * (nLat - 1) * nLon + 3 * nLon + 3 * phiIdx + 1] =
        static_cast<unsigned int>((nLat - 1) * nLon + (phiIdx + 1) % nLon);
    I[6 * (nLat - 1) * nLon + 3 * nLon + 3 * phiIdx + 2] =
        static_cast<unsigned int>((nLat - 1) * nLon + phiIdx);
  }
}

atb::SurfaceGeometry SphereMarkerOpenGlRenderingViewRenderer::_marker;
