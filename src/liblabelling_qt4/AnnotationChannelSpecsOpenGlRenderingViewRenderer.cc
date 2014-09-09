/**************************************************************************
**       Title: OpenGlRendering-View Annotation-Channel Renderer
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    This class is the interface between the different View- and Channel
**    types. For each Channel type and each View type one such interface
**    class needs to be implemented. This is the specific implementation
**    for AnnotationChannelSpecs <=> OpenGlRenderingView
**
**************************************************************************/

#include "AnnotationChannelSpecsOpenGlRenderingViewRenderer.hh"

#include <QtGui/QPainter>

#include "MarkerRenderer.hh"

#include "MultiChannelModel.hh"
#include "AnnotationChannelSpecs.hh"
#include "OpenGlRenderingViewWidget.hh"

AnnotationChannelSpecsOpenGlRenderingViewRenderer::
AnnotationChannelSpecsOpenGlRenderingViewRenderer(
    AnnotationChannelSpecs* channel, OpenGlRenderingViewWidget* view)
        : ChannelSpecsOpenGlRenderingViewRenderer(channel, view)
{}
  
AnnotationChannelSpecsOpenGlRenderingViewRenderer::
~AnnotationChannelSpecsOpenGlRenderingViewRenderer()
{}

void AnnotationChannelSpecsOpenGlRenderingViewRenderer::render(
    QPainter* painter) const
{
  if (painter == NULL || !painter->isActive()) return;
  if (p_view != NULL && !p_view->isVisible()) return;
  if (p_channel != NULL && !p_channel->visible()) return;

  AnnotationChannelSpecs* channel =
      static_cast<AnnotationChannelSpecs*>(p_channel);
  
  for (std::vector<Marker*>::const_iterator mIt = channel->begin();
       mIt != channel->end(); ++mIt)
  {
    if ((*mIt)->renderer(p_view) != NULL)
    {
      if (static_cast<OpenGlRenderingViewWidget*>(p_view)->positionVisible(
              (*mIt)->positionUm())) (*mIt)->renderer(p_view)->render(painter);
    }
    else 
        std::cerr << "Warning: Marker at " << (*mIt)->positionUm()
                  << " could not be rendered. No associated renderer found"
                  << std::endl;
  }
}

