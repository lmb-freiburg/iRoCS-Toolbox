/**************************************************************************
**       Title: CylinderMarker-OrthoView Renderer
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    This class is the interface between the different View- and Marker
**    types. For each Marker type and each View type one such interface
**    class needs to be implemented. This is the specific implementation
**    for CylinderMarker <=> OrthoView
**
**************************************************************************/

#ifndef CYLINDERMARKERORTHOVIEWRENDERER_HH
#define CYLINDERMARKERORTHOVIEWRENDERER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "MarkerOrthoViewRenderer.hh"

class CylinderMarker;
class OrthoViewWidget;

class CylinderMarkerOrthoViewRenderer : public MarkerOrthoViewRenderer
{

Q_OBJECT

public:
  
  CylinderMarkerOrthoViewRenderer(
      CylinderMarker* marker, OrthoViewWidget* view);
  CylinderMarkerOrthoViewRenderer(
      CylinderMarkerOrthoViewRenderer const &renderer);
  virtual ~CylinderMarkerOrthoViewRenderer();
  
  virtual MarkerRenderer *clone() const;

  virtual void render(QPainter* painter = NULL) const;
  virtual void render(
      QXmlStreamWriter &svgStream,
      blitz::TinyVector<double,3> const &shapePx,
      blitz::TinyVector<double,3> const &lowerBoundUm,
      blitz::TinyVector<double,3> const &upperBoundUm,
      std::string const &font, double fontSizePt,
      iRoCS::ProgressReporter *pr = NULL) const;

  virtual void userInteractionEvent(UserInteractionEvent *event);

};

#endif
