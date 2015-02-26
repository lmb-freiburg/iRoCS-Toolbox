/**************************************************************************
**       Title: Abstract Channel OpenGL 3D Renderer
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    This class is the interface between the OpenGlRenderingView and different
**    Channel types. For each Channel type one such interface class needs to be
**    implemented.
**
**************************************************************************/

#ifndef CHANNELSPECSOPENGLRENDERINGVIEWRENDERER_HH
#define CHANNELSPECSOPENGLRENDERINGVIEWRENDERER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "ChannelSpecsRenderer.hh"

class OpenGlRenderingViewWidget;

class ChannelSpecsOpenGlRenderingViewRenderer : public ChannelSpecsRenderer
{

Q_OBJECT

public:
  
  ChannelSpecsOpenGlRenderingViewRenderer(
      ChannelSpecs* channel, OpenGlRenderingViewWidget* view);
  virtual ~ChannelSpecsOpenGlRenderingViewRenderer();

};

#endif
