/**************************************************************************
**       Title: OpenGlRenderingView Data-Channel Renderer
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    This class is the interface between the different View- and Channel
**    types. For each Channel type and each View type one such interface
**    class needs to be implemented. This is the specific implementation
**    for DataChannelSpecs <=> OpenGlRenderingView
**
**************************************************************************/

#ifndef DATACHANNELSPECSOPENGLRENDERINGVIEWRENDERER_HH
#define DATACHANNELSPECSOPENGLRENDERINGVIEWRENDERER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "ChannelSpecsOpenGlRenderingViewRenderer.hh"

class DataChannelSpecs;
class OpenGlRenderingViewWidget;

class DataChannelSpecsOpenGlRenderingViewRenderer :
    public ChannelSpecsOpenGlRenderingViewRenderer
{

Q_OBJECT

public:
  
  DataChannelSpecsOpenGlRenderingViewRenderer(
      DataChannelSpecs* channel, OpenGlRenderingViewWidget* view);
  ~DataChannelSpecsOpenGlRenderingViewRenderer();
  
  void render(QPainter* painter = NULL) const;

};

#endif
