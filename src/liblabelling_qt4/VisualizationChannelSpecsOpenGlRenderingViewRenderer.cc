/**************************************************************************
**       Title: OpenGlRendering-View Visualization-Channel Renderer
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    This class is the interface between the different View- and Channel
**    types. For each Channel type and each View type one such interface
**    class needs to be implemented. This is the specific implementation
**    for VisualizationChannelSpecs <=> OpenGlRenderingView
**
**************************************************************************/

#include <QtGui/QMainWindow>
#include <QtGui/QStatusBar>

#include <libArrayToolbox/Array.hh>
#include <libArrayToolbox/MarchingCubes.hh>

#include "VisualizationChannelSpecsOpenGlRenderingViewRenderer.hh"

#include "MultiChannelModel.hh"
#include "VisualizationChannelSpecs.hh"
#include "OpenGlRenderingViewWidget.hh"
#include "OpenGlRenderingWidget.hh"
#include "ColorMap.hh"

#include <GL/gl.h>

#ifdef _OPENMP
#include <omp.h>
#endif

VisualizationChannelSpecsOpenGlRenderingViewRenderer::
VisualizationChannelSpecsOpenGlRenderingViewRenderer(
    VisualizationChannelSpecs* channel, OpenGlRenderingViewWidget* view)
        : ChannelSpecsOpenGlRenderingViewRenderer(channel, view)
{}
  
VisualizationChannelSpecsOpenGlRenderingViewRenderer::
~VisualizationChannelSpecsOpenGlRenderingViewRenderer()
{}

void VisualizationChannelSpecsOpenGlRenderingViewRenderer::render(
    QPainter*) const
{
  // Not implemented
}

