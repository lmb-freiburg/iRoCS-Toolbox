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

#ifndef SURFACEMARKEROPENGLRENDERINGVIEWRENDERER_HH
#define SURFACEMARKEROPENGLRENDERINGVIEWRENDERER_HH

#include "MarkerOpenGlRenderingViewRenderer.hh"

class SurfaceMarker;
class OpenGlRenderingViewWidget;

class SurfaceMarkerOpenGlRenderingViewRenderer :
    public MarkerOpenGlRenderingViewRenderer
{

Q_OBJECT

public:
  
  SurfaceMarkerOpenGlRenderingViewRenderer(
      SurfaceMarker* marker, OpenGlRenderingViewWidget* view);
  SurfaceMarkerOpenGlRenderingViewRenderer(
      SurfaceMarkerOpenGlRenderingViewRenderer const &renderer);
  virtual ~SurfaceMarkerOpenGlRenderingViewRenderer();
  
  virtual MarkerRenderer *clone() const;

  virtual void render(QPainter* painter = NULL) const;

  virtual void userInteractionEvent(UserInteractionEvent *event);

};

#endif
