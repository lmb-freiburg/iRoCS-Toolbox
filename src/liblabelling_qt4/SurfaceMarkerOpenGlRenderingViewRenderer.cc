/**************************************************************************
**       Title: SurfaceMarker-OpenGlRenderingView Renderer
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    This class is the interface between the different View- and Marker
**    types. For each Marker type and each View type one such interface
**    class needs to be implemented. This is the specific implementation
**    for SurfaceMarker <=> OpenGlRenderingView
**
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
