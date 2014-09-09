/**************************************************************************
**       Title: CellMarker-OpenGlRenderingView Renderer
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    This class is the interface between the different View- and Marker
**    types. For each Marker type and each View type one such interface
**    class needs to be implemented. This is the specific implementation
**    for CellMarker <=> OpenGlRenderingView
**
**************************************************************************/

#ifndef CELLMARKEROPENGLRENDERINGVIEWRENDERER_HH
#define CELLMARKEROPENGLRENDERINGVIEWRENDERER_HH

#include "SurfaceMarkerOpenGlRenderingViewRenderer.hh"

class CellMarker;
class OpenGlRenderingViewWidget;

class CellMarkerOpenGlRenderingViewRenderer :
    public SurfaceMarkerOpenGlRenderingViewRenderer
{

Q_OBJECT

public:
  
  CellMarkerOpenGlRenderingViewRenderer(
      CellMarker* marker, OpenGlRenderingViewWidget* view);
  CellMarkerOpenGlRenderingViewRenderer(
      CellMarkerOpenGlRenderingViewRenderer const &renderer);
  virtual ~CellMarkerOpenGlRenderingViewRenderer();
  
  virtual MarkerRenderer *clone() const;

};

#endif
