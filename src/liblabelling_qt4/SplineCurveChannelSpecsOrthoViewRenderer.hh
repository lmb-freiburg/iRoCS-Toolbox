/**************************************************************************
**       Title: Ortho-View SplineCurve-Channel Renderer
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    This class is the interface between the different View- and Channel
**    types. For each Channel type and each View type one such interface
**    class needs to be implemented. This is the specific implementation
**    for SplineCurveChannelSpecs <=> OrthoView
**
**************************************************************************/

#ifndef SPLINECURVECHANNELSPECSORTHOVIEWRENDERER_HH
#define SPLINECURVECHANNELSPECSORTHOVIEWRENDERER_HH

#include "ChannelSpecsOrthoViewRenderer.hh"

class SplineCurveChannelSpecs;
class OrthoViewWidget;

class SplineCurveChannelSpecsOrthoViewRenderer :
    public ChannelSpecsOrthoViewRenderer
{

public:
  
  SplineCurveChannelSpecsOrthoViewRenderer(
      SplineCurveChannelSpecs* channel, OrthoViewWidget* view);
  ~SplineCurveChannelSpecsOrthoViewRenderer();
  
  void userInteractionEvent(const UserInteractionEvent& event);
  void render(QPainter* painter = NULL) const;
  void render(
      QXmlStreamWriter &svgStream,
      blitz::TinyVector<double,3> const &shapePx,
      blitz::TinyVector<double,3> const &lowerBoundUm,
      blitz::TinyVector<double,3> const &upperBoundUm,
      std::string const &font, double fontSizePt,
      iRoCS::ProgressReporter *pr = NULL) const;

private:
  
  bool _startDragging, _isDragging;
  int _draggedControlPoint;

};

#endif
