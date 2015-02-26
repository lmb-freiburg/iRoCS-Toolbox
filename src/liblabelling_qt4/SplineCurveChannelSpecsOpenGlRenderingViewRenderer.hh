/**************************************************************************
**       Title: OpenGlRenderingView SplineCurve-Channel Renderer
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    This class is the interface between the different View- and Channel
**    types. For each Channel type and each View type one such interface
**    class needs to be implemented. This is the specific implementation
**    for SplineCurveChannelSpecs <=> OpenGlRenderingView
**
**************************************************************************/

#ifndef SPLINECURVECHANNELSPECSOPENGLRENDERINGVIEWRENDERER_HH
#define SPLINECURVECHANNELSPECSOPENGLRENDERINGVIEWRENDERER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "ChannelSpecsOpenGlRenderingViewRenderer.hh"
#include <libArrayToolbox/SurfaceGeometry.hh>

class SplineCurveChannelSpecs;
class OpenGlRenderingViewWidget;

class SplineCurveChannelSpecsOpenGlRenderingViewRenderer :
    public ChannelSpecsOpenGlRenderingViewRenderer
{

Q_OBJECT

public:
  
  SplineCurveChannelSpecsOpenGlRenderingViewRenderer(
      SplineCurveChannelSpecs* channel, OpenGlRenderingViewWidget* view);
  ~SplineCurveChannelSpecsOpenGlRenderingViewRenderer();
  
  void render(QPainter* painter = NULL) const;

  atb::SurfaceGeometry const &curveGeometry() const;

  void updateCache() const;

private:

  mutable atb::SurfaceGeometry _curve;

};

#endif
