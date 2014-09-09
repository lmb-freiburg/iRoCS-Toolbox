/**************************************************************************
**       Title: OpenGlRenderingView IRoCS-Channel Renderer for elliptic root
**              profiles
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    This class is the interface between the different View- and Channel
**    types. For each Channel type and each View type one such interface
**    class needs to be implemented. This is the specific implementation
**    for IRoCSSCTChannelSpecs <=> OpenGlRenderingView
**
**************************************************************************/

#ifndef IROCSSCTCHANNELSPECSOPENGLRENDERINGVIEWRENDERER_HH
#define IROCSSCTCHANNELSPECSOPENGLRENDERINGVIEWRENDERER_HH

#include "ChannelSpecsOpenGlRenderingViewRenderer.hh"
#include <libArrayToolbox/SurfaceGeometry.hh>

class OpenGlRenderingViewWidget;
class IRoCSSCTChannelSpecs;

class IRoCSSCTChannelSpecsOpenGlRenderingViewRenderer :
    public ChannelSpecsOpenGlRenderingViewRenderer
{

Q_OBJECT

public:
  
  IRoCSSCTChannelSpecsOpenGlRenderingViewRenderer(
      IRoCSSCTChannelSpecs* channel, OpenGlRenderingViewWidget* view);
  ~IRoCSSCTChannelSpecsOpenGlRenderingViewRenderer();
  
  void render(QPainter* painter = NULL) const;

  atb::SurfaceGeometry const &axisGeometry() const;

  void updateCache() const;

private:

  mutable atb::SurfaceGeometry _axis;

};

#endif
