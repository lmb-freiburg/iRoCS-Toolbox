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

#include "ChannelSpecsOpenGlRenderingViewRenderer.hh"

#include "OpenGlRenderingViewWidget.hh"

ChannelSpecsOpenGlRenderingViewRenderer::
ChannelSpecsOpenGlRenderingViewRenderer(
    ChannelSpecs* channel, OpenGlRenderingViewWidget* view)
        : ChannelSpecsRenderer(channel, view)
{}

ChannelSpecsOpenGlRenderingViewRenderer::
~ChannelSpecsOpenGlRenderingViewRenderer()
{}
