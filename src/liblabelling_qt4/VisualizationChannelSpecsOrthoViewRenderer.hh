/**************************************************************************
**       Title: Ortho-View Visualization-Channel Renderer
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    This class is the interface between the different View- and Channel
**    types. For each Channel type and each View type one such interface
**    class needs to be implemented. This is the specific implementation
**    for VisualizationChannelSpecs <=> OrthoView
**
**************************************************************************/

#ifndef VISUALIZATIONCHANNELSPECSORTHOVIEWRENDERER_HH
#define VISUALIZATIONCHANNELSPECSORTHOVIEWRENDERER_HH

#include "ChannelSpecsOrthoViewRenderer.hh"

#include <libArrayToolbox/Array.hh>

class VisualizationChannelSpecs;
class OrthoViewWidget;

class VisualizationChannelSpecsOrthoViewRenderer :
      public ChannelSpecsOrthoViewRenderer
{

Q_OBJECT

public:

  VisualizationChannelSpecsOrthoViewRenderer(
    VisualizationChannelSpecs* channel, OrthoViewWidget* view);
  ~VisualizationChannelSpecsOrthoViewRenderer();

  void render(QPainter* painter = NULL) const;
  void render(
      QXmlStreamWriter &svgStream,
      blitz::TinyVector<double,3> const &shapePx,
      blitz::TinyVector<double,3> const &lowerBoundUm,
      blitz::TinyVector<double,3> const &upperBoundUm,
      std::string const &font, double fontSizePt,
      iRoCS::ProgressReporter *pr = NULL) const;

  blitz::TinyVector<atb::BlitzIndexT,3> cacheOffsetPx() const;
  blitz::TinyVector<atb::BlitzIndexT,3> cacheShapePx() const;
  blitz::Array<blitz::TinyVector<float,3>,2> const &cache(int direction) const;
                                                 
public slots:
  
  virtual void updateCache(int direction) const;

private:
  
  mutable blitz::TinyVector<blitz::Array<blitz::TinyVector<float,3>,2>,3>
  _cache;

};

#endif
