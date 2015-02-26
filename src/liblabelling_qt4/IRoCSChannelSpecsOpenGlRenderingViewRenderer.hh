/**************************************************************************
**       Title: OpenGlRenderingView IRoCS-Channel Renderer
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    This class is the interface between the different View- and Channel
**    types. For each Channel type and each View type one such interface
**    class needs to be implemented. This is the specific implementation
**    for IRoCSChannelSpecs <=> OpenGlRenderingView
**
**************************************************************************/

#ifndef IROCSCHANNELSPECSOPENGLRENDERINGVIEWRENDERER_HH
#define IROCSCHANNELSPECSOPENGLRENDERINGVIEWRENDERER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "ChannelSpecsOpenGlRenderingViewRenderer.hh"
#include <libArrayToolbox/SurfaceGeometry.hh>

class OpenGlRenderingViewWidget;
class IRoCSChannelSpecs;

class IRoCSChannelSpecsOpenGlRenderingViewRenderer :
    public ChannelSpecsOpenGlRenderingViewRenderer
{

Q_OBJECT

public:
  
  IRoCSChannelSpecsOpenGlRenderingViewRenderer(
      IRoCSChannelSpecs* channel, OpenGlRenderingViewWidget* view);
  ~IRoCSChannelSpecsOpenGlRenderingViewRenderer();
  
  void render(QPainter* painter = NULL) const;

  atb::SurfaceGeometry const &axisGeometry() const;
  atb::SurfaceGeometry const &boundaryGeometry() const;

  void updateCache() const;

private:

  mutable atb::SurfaceGeometry _axis;
  mutable atb::SurfaceGeometry _boundary;

};

#endif
