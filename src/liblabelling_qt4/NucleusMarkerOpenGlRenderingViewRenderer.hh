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

#ifndef NUCLEUSMARKEROPENGLRENDERINGVIEWRENDERER_HH
#define NUCLEUSMARKEROPENGLRENDERINGVIEWRENDERER_HH

#include "SHSurfaceMarkerOpenGlRenderingViewRenderer.hh"

class NucleusMarker;
class OpenGlRenderingViewWidget;

class NucleusMarkerOpenGlRenderingViewRenderer :
    public SHSurfaceMarkerOpenGlRenderingViewRenderer
{

Q_OBJECT

public:
  
  NucleusMarkerOpenGlRenderingViewRenderer(
      NucleusMarker* marker, OpenGlRenderingViewWidget* view);
  NucleusMarkerOpenGlRenderingViewRenderer(
      NucleusMarkerOpenGlRenderingViewRenderer const &renderer);
  ~NucleusMarkerOpenGlRenderingViewRenderer();
  
  MarkerRenderer *clone() const;

  void render(QPainter* painter = NULL) const;

  void userInteractionEvent(UserInteractionEvent *event);

};

#endif
