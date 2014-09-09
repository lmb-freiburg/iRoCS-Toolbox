/**************************************************************************
**       Title: PointMarker-OrthoView Renderer
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    This class is the interface between the different View- and Marker
**    types. For each Marker type and each View type one such interface
**    class needs to be implemented. This is the specific implementation
**    for PointMarker <=> OrthoView
**
**************************************************************************/

#ifndef POINTMARKERORTHOVIEWRENDERER_HH
#define POINTMARKERORTHOVIEWRENDERER_HH

#include "MarkerOrthoViewRenderer.hh"

#include <QtCore/QLineF>

class PointMarker;
class OrthoViewWidget;


/*======================================================================*/
/*!
 *  \class PointMarkerOrthoViewRendererCache PointMarkerOrthoViewRenderer.hh "PointMarkerOrthoViewRenderer.hh"
 *  \brief The PointMarkerOrthoViewRendererCache class stores and replays
 *    point marker geometry information using a QPainter.
 */
/*======================================================================*/
class PointMarkerOrthoViewRendererCache : public MarkerOrthoViewRendererCache
{

public:
  
  PointMarkerOrthoViewRendererCache();
  PointMarkerOrthoViewRendererCache(
      PointMarkerOrthoViewRendererCache const &cache);
  virtual ~PointMarkerOrthoViewRendererCache();

/*======================================================================*/
/*! 
 *   Get a new copy of this cache
 *
 *   \return The address to the clone of this cache.
 */
/*======================================================================*/
  virtual MarkerOrthoViewRendererCache *clone() const;
  
/*======================================================================*/
/*! 
 *   Render the content of the cache into the given painter. This function
 *   is called from the MarkerOrthoViewRenderer::render(QPainter*) method at
 *   every repaint. Pen color and width are already set when this function is
 *   called to reflect label and selection status, so change these pen
 *   properties only if you want to highlight or demote parts of the
 *   visualization.
 *
 *   \param painter The painter to pass painter commands to
 */
/*======================================================================*/
  virtual void render(QPainter *painter) const;

/*======================================================================*/
/*! 
 *   Set the cross lines to the given lines. This implicitely changes
 *   the rendering mode to cross rendering.
 *
 *   \param cross The QLines defining the cross to draw.
 */
/*======================================================================*/
  void setCross(blitz::TinyVector<QLineF,2> const &cross);

/*======================================================================*/
/*! 
 *   Set the point to the given QPoint. This implicitely changes
 *   the rendering mode to point rendering.
 *
 *   \param point The QPoint to draw.
 */
/*======================================================================*/
  void setPoint(QPointF const &point);

protected:
  
  bool _showCross;
  blitz::TinyVector<QLineF,2> _cross;
  QPointF _point;

};


class PointMarkerOrthoViewRenderer : public MarkerOrthoViewRenderer
{

Q_OBJECT

public:
  
  PointMarkerOrthoViewRenderer(PointMarker* marker, OrthoViewWidget* view);
  PointMarkerOrthoViewRenderer(PointMarkerOrthoViewRenderer const &renderer);
  virtual ~PointMarkerOrthoViewRenderer();
  
  virtual MarkerRenderer *clone() const;

  virtual void render(
      QXmlStreamWriter &svgStream,
      blitz::TinyVector<double,3> const &shapePx,
      blitz::TinyVector<double,3> const &lowerBoundUm,
      blitz::TinyVector<double,3> const &upperBoundUm,
      std::string const &font, double fontSizePt,
      iRoCS::ProgressReporter *pr = NULL) const;

  virtual void userInteractionEvent(UserInteractionEvent *event);

public slots:

/*======================================================================*/
/*! 
 *   Update the rendering cache for the specified orthoview dimension. This
 *   function is called by the updateCache()-Method for every direction.
 *   On crosshair position change the view widget explicitely calls this
 *   function with the dimensions orthogonal to the cross hair position
 *   change. Reimplement this function if you have expensive computations
 *   that should not be done at every repaint. The default implementation only
 *   calls the update() function of the view. You can disable the cacheUpdates
 *   using the setCacheUpdatesEnabled() function. This is important if you
 *   programmatically perform many operations on the Marker of which
 *   each would need a cache update. After the sequence of operations, you
 *   have to call updateCache() explicitely to apply the changes. After cache
 *   update update() of the view is called to visualize the changes. So if
 *   many markers/channels are involved you should call setUpdatesEnabled(false)
 *   to the view before these changes.
 */
/*======================================================================*/
  virtual void updateCache(int direction) const;

};

#endif
