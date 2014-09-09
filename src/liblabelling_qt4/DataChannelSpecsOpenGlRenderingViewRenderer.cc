/**************************************************************************
**       Title: OpenGlRendering-View Data-Channel Renderer
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

#include <QtGui/QFont>

#include "DataChannelSpecsOpenGlRenderingViewRenderer.hh"

#include "MultiChannelModel.hh"
#include "DataChannelSpecs.hh"
#include "OpenGlRenderingViewWidget.hh"

#ifdef _OPENMP
#include <omp.h>
#endif

DataChannelSpecsOpenGlRenderingViewRenderer::
DataChannelSpecsOpenGlRenderingViewRenderer(
    DataChannelSpecs* channel, OpenGlRenderingViewWidget* view)
        : ChannelSpecsOpenGlRenderingViewRenderer(channel, view)
{}
  
DataChannelSpecsOpenGlRenderingViewRenderer::
~DataChannelSpecsOpenGlRenderingViewRenderer()
{}

void DataChannelSpecsOpenGlRenderingViewRenderer::render(QPainter*) const
{
  // Not implemented: Here a volume renderer has to be implemented
}

