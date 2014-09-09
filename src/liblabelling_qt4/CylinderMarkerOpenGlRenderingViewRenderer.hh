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

#ifndef CYLINDERMARKEROPENGLRENDERINGVIEWRENDERER_HH
#define CYLINDERMARKEROPENGLRENDERINGVIEWRENDERER_HH

#include "MarkerOpenGlRenderingViewRenderer.hh"

class CylinderMarker;
class OpenGlRenderingViewWidget;

class CylinderMarkerOpenGlRenderingViewRenderer :
    public MarkerOpenGlRenderingViewRenderer
{

Q_OBJECT

public:
  
  CylinderMarkerOpenGlRenderingViewRenderer(
      CylinderMarker* marker, OpenGlRenderingViewWidget* view);
  CylinderMarkerOpenGlRenderingViewRenderer(
      CylinderMarkerOpenGlRenderingViewRenderer const &renderer);
  ~CylinderMarkerOpenGlRenderingViewRenderer();
  
  MarkerRenderer *clone() const;

  void render(QPainter* painter = NULL) const;

  void userInteractionEvent(UserInteractionEvent *event);

};

#endif
