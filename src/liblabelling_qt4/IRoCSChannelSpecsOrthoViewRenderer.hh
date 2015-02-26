/**************************************************************************
**       Title: Ortho-View iRoCS-Channel Renderer
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    This class is the interface between the different View- and Channel
**    types. For each Channel type and each View type one such interface
**    class needs to be implemented. This is the specific implementation
**    for IRoCSChannelSpecs <=> OrthoView
**
**************************************************************************/

#ifndef IROCSCHANNELSPECSORTHOVIEWRENDERER_HH
#define IROCSCHANNELSPECSORTHOVIEWRENDERER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "ChannelSpecsOrthoViewRenderer.hh"

class IRoCSChannelSpecs;
class OrthoViewWidget;

class IRoCSChannelSpecsOrthoViewRenderer : public ChannelSpecsOrthoViewRenderer
{

public:
  
  IRoCSChannelSpecsOrthoViewRenderer(
      IRoCSChannelSpecs* channel, OrthoViewWidget* view);
  ~IRoCSChannelSpecsOrthoViewRenderer();
  
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
