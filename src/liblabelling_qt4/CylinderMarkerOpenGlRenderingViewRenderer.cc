/**************************************************************************
**       Title: CylinderMarker-OpenGlRenderingView Renderer
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    This class is the interface between the different View- and Marker
**    types. For each Marker type and each View type one such interface
**    class needs to be implemented. This is the specific implementation
**    for CylinderMarker <=> OpenGlRenderingView
**
**************************************************************************/

#include "CylinderMarkerOpenGlRenderingViewRenderer.hh"

#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>

#include "MultiChannelModel.hh"
#include "AnnotationChannelSpecs.hh"

#include "CylinderMarker.hh"
#include "OpenGlRenderingViewWidget.hh"

#include "UserInteractionEvent.hh"

CylinderMarkerOpenGlRenderingViewRenderer::
CylinderMarkerOpenGlRenderingViewRenderer(
    CylinderMarker* marker, OpenGlRenderingViewWidget* view)
        : MarkerOpenGlRenderingViewRenderer(marker, view)
{}

CylinderMarkerOpenGlRenderingViewRenderer::
CylinderMarkerOpenGlRenderingViewRenderer(
    CylinderMarkerOpenGlRenderingViewRenderer const &renderer)
        : MarkerOpenGlRenderingViewRenderer(renderer)
{}

CylinderMarkerOpenGlRenderingViewRenderer::
~CylinderMarkerOpenGlRenderingViewRenderer()
{}
  
MarkerRenderer *CylinderMarkerOpenGlRenderingViewRenderer::clone() const
{
  return new CylinderMarkerOpenGlRenderingViewRenderer(*this);
}

void CylinderMarkerOpenGlRenderingViewRenderer::render(QPainter*) const
{
  if (p_marker->channel() == NULL)
  {
    std::cerr << "CylinderMarkerOpenGlRenderingViewRenderer::render(): "
              << "ERROR: Marker at position " << p_marker->positionUm()
              << " has no asociated channel!" << std::endl;
    return;
  }

  // To be implemented
  std::cerr << __FILE__ << ":" << __LINE__ << ": Implement me" << std::endl;
}

void CylinderMarkerOpenGlRenderingViewRenderer::userInteractionEvent(
    UserInteractionEvent *)
{}
