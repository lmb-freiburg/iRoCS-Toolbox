/**************************************************************************
**       Title: OpenGlRendering-View RGB-Channel Renderer
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

#include <QtGui/QFont>

#include "RGBChannelSpecsOpenGlRenderingViewRenderer.hh"

#include "MultiChannelModel.hh"
#include "RGBChannelSpecs.hh"
#include "OpenGlRenderingViewWidget.hh"

#ifdef _OPENMP
#include <omp.h>
#endif

RGBChannelSpecsOpenGlRenderingViewRenderer::
RGBChannelSpecsOpenGlRenderingViewRenderer(
    RGBChannelSpecs* channel, OpenGlRenderingViewWidget* view)
        : ChannelSpecsOpenGlRenderingViewRenderer(channel, view)
{}
  
RGBChannelSpecsOpenGlRenderingViewRenderer::
~RGBChannelSpecsOpenGlRenderingViewRenderer()
{}

void RGBChannelSpecsOpenGlRenderingViewRenderer::render(QPainter*) const
{
  // Not implemented: Here a volume renderer has to be implemented
}

