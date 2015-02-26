/**************************************************************************
**       Title: SHSurfaceMarker-OpenGlRenderingView Renderer
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    This class is the interface between the different View- and Marker
**    types. For each Marker type and each View type one such interface
**    class needs to be implemented. This is the specific implementation
**    for SHSurfaceMarker <=> OpenGlRenderingView
**
**************************************************************************/

#ifndef SHSURFACEMARKEROPENGLRENDERINGVIEWRENDERER_HH
#define SHSURFACEMARKEROPENGLRENDERINGVIEWRENDERER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "SphereMarkerOpenGlRenderingViewRenderer.hh"

class SHSurfaceMarker;
class OpenGlRenderingViewWidget;

class SHSurfaceMarkerOpenGlRenderingViewRenderer :
    public SphereMarkerOpenGlRenderingViewRenderer
{

Q_OBJECT

public:
  
  SHSurfaceMarkerOpenGlRenderingViewRenderer(
      SHSurfaceMarker* marker, OpenGlRenderingViewWidget* view);
  SHSurfaceMarkerOpenGlRenderingViewRenderer(
      SHSurfaceMarkerOpenGlRenderingViewRenderer const &renderer);
  ~SHSurfaceMarkerOpenGlRenderingViewRenderer();
  
  MarkerRenderer *clone() const;

  void render(QPainter* painter = NULL) const;

  void userInteractionEvent(UserInteractionEvent *event);

};

#endif
