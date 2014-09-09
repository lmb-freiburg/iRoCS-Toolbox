/**************************************************************************
**       Title: NucleusMarker-OpenGlRenderingView Renderer
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    This class is the interface between the different View- and Marker
**    types. For each Marker type and each View type one such interface
**    class needs to be implemented. This is the specific implementation
**    for NucleusMarker <=> OpenGlRenderingView
**
**************************************************************************/

#include "NucleusMarkerOpenGlRenderingViewRenderer.hh"

#include <QtGui/QPainter>
#include <QtGui/QPicture>
#include <QtGui/QMouseEvent>
#include <QtXml/QXmlStreamWriter>

#include "MultiChannelModel.hh"
#include "AnnotationChannelSpecs.hh"

#include "NucleusMarker.hh"
#include "OpenGlRenderingViewWidget.hh"

#include "UserInteractionEvent.hh"

NucleusMarkerOpenGlRenderingViewRenderer::
NucleusMarkerOpenGlRenderingViewRenderer(
    NucleusMarker* marker, OpenGlRenderingViewWidget* view)
        : SHSurfaceMarkerOpenGlRenderingViewRenderer(marker, view)
{}

NucleusMarkerOpenGlRenderingViewRenderer::
NucleusMarkerOpenGlRenderingViewRenderer(
    NucleusMarkerOpenGlRenderingViewRenderer const &renderer)
        : SHSurfaceMarkerOpenGlRenderingViewRenderer(renderer)
{}

NucleusMarkerOpenGlRenderingViewRenderer::
~NucleusMarkerOpenGlRenderingViewRenderer()
{}
  
MarkerRenderer *NucleusMarkerOpenGlRenderingViewRenderer::clone() const
{
  return new NucleusMarkerOpenGlRenderingViewRenderer(*this);
}

void NucleusMarkerOpenGlRenderingViewRenderer::render(QPainter *painter) const
{
  if (painter == NULL || !painter->isActive()) return;
  if (p_marker->channel() == NULL)
  {
    std::cerr << "NucleusMarkerOpenGlRenderingViewRenderer::render(): "
              << "ERROR: Marker at position " << p_marker->positionUm()
              << " has no asociated channel!" << std::endl;
    return;
  }

  // To be implemented, currently falling back to SHSurfaceMarker

  SHSurfaceMarkerOpenGlRenderingViewRenderer::render(painter);
}

void NucleusMarkerOpenGlRenderingViewRenderer::userInteractionEvent(
    UserInteractionEvent *)
{}
