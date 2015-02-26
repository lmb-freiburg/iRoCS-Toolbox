/**************************************************************************
**       Title: PointMarker-OpenGlRenderingView Renderer
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    This class is the interface between the different View- and Marker
**    types. For each Marker type and each View type one such interface
**    class needs to be implemented. This is the specific implementation
**    for PointMarker <=> OpenGlRenderingView
**
**************************************************************************/

#ifndef POINTMARKEROPENGLRENDERINGVIEWRENDERER_HH
#define POINTMARKEROPENGLRENDERINGVIEWRENDERER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "MarkerOpenGlRenderingViewRenderer.hh"

#include <libArrayToolbox/SurfaceGeometry.hh>

class OpenGlRenderingViewWidget;

class PointMarker;

class PointMarkerOpenGlRenderingViewRenderer :
    public MarkerOpenGlRenderingViewRenderer
{

Q_OBJECT

public:
  
  PointMarkerOpenGlRenderingViewRenderer(
      PointMarker* marker, OpenGlRenderingViewWidget* view);
  PointMarkerOpenGlRenderingViewRenderer(
      PointMarkerOpenGlRenderingViewRenderer const &renderer);
  ~PointMarkerOpenGlRenderingViewRenderer();
  
  MarkerRenderer *clone() const;

  void render(QPainter* painter = NULL) const;

  void userInteractionEvent(UserInteractionEvent *event);

  atb::SurfaceGeometry const &geometry() const;

private:
  
  static void initMarker();
  static atb::SurfaceGeometry _marker;

};

#endif
