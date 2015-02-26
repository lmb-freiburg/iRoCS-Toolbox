/**************************************************************************
**       Title: OpenGlRenderingView Annotation-Channel Renderer
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

#ifndef ANNOTATIONCHANNELSPECSOPENGLRENDERINGVIEWRENDERER_HH
#define ANNOTATIONCHANNELSPECSOPENGLRENDERINGVIEWRENDERER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "ChannelSpecsOpenGlRenderingViewRenderer.hh"

class AnnotationChannelSpecs;
class OpenGlRenderingViewWidget;

class AnnotationChannelSpecsOpenGlRenderingViewRenderer :
    public ChannelSpecsOpenGlRenderingViewRenderer
{

Q_OBJECT

public:
  
  AnnotationChannelSpecsOpenGlRenderingViewRenderer(
      AnnotationChannelSpecs* channel, OpenGlRenderingViewWidget* view);
  ~AnnotationChannelSpecsOpenGlRenderingViewRenderer();
  
  void render(QPainter* painter = NULL) const;

};

#endif
