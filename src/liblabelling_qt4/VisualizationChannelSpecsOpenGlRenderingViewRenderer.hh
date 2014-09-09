/**************************************************************************
**       Title: OpenGlRenderingView Visualization-Channel Renderer
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

#ifndef VISUALIZATIONCHANNELSPECSOPENGLRENDERINGVIEWRENDERER_HH
#define VISUALIZATIONCHANNELSPECSOPENGLRENDERINGVIEWRENDERER_HH

#include "ChannelSpecsOpenGlRenderingViewRenderer.hh"

#include <map>

class QMainWindow;
class QLabel;
class QProgressBar;
class QPushButton;

class VisualizationChannelSpecs;
class OpenGlRenderingViewWidget;

class VisualizationChannelSpecsOpenGlRenderingViewRenderer :
    public ChannelSpecsOpenGlRenderingViewRenderer
{

Q_OBJECT

public:
  
  VisualizationChannelSpecsOpenGlRenderingViewRenderer(
      VisualizationChannelSpecs* channel, OpenGlRenderingViewWidget* view);
  ~VisualizationChannelSpecsOpenGlRenderingViewRenderer();
  
  void render(QPainter* painter = NULL) const;

};

#endif
