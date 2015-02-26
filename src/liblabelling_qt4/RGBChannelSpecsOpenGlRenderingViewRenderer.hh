/**************************************************************************
**       Title: OpenGlRenderingView RGB-Channel Renderer
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    This class is the interface between the different View- and Channel
**    types. For each Channel type and each View type one such interface
**    class needs to be implemented. This is the specific implementation
**    for RGBChannelSpecs <=> OpenGlRenderingView
**
**************************************************************************/

#ifndef RGBCHANNELSPECSOPENGLRENDERINGVIEWRENDERER_HH
#define RGBCHANNELSPECSOPENGLRENDERINGVIEWRENDERER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "ChannelSpecsOpenGlRenderingViewRenderer.hh"

class RGBChannelSpecs;
class OpenGlRenderingViewWidget;

class RGBChannelSpecsOpenGlRenderingViewRenderer :
    public ChannelSpecsOpenGlRenderingViewRenderer
{

Q_OBJECT

public:
  
  RGBChannelSpecsOpenGlRenderingViewRenderer(
      RGBChannelSpecs* channel, OpenGlRenderingViewWidget* view);
  ~RGBChannelSpecsOpenGlRenderingViewRenderer();
  
  void render(QPainter* painter = NULL) const;

};

#endif
