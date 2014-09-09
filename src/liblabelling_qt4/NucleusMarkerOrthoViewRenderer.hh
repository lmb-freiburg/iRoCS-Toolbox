/**************************************************************************
**       Title: NucleusMarker-OrthoView Renderer
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    This class is the interface between the different View- and Marker
**    types. For each Marker type and each View type one such interface
**    class needs to be implemented. This is the specific implementation
**    for NucleusMarker <=> OrthoView
**
**************************************************************************/

#ifndef NUCLEUSMARKERORTHOVIEWRENDERER_HH
#define NUCLEUSMARKERORTHOVIEWRENDERER_HH

#include "SHSurfaceMarkerOrthoViewRenderer.hh"

class NucleusMarker;
class OrthoViewWidget;

/*======================================================================*/
/*!
 *  \class SHSurfaceMarkerOrthoViewRendererCache SphereMarkerOrthoViewRenderer.hh "SphereMarkerOrthoViewRenderer.hh"
 *  \brief The SHSurfaceMarkerOrthoViewRendererCache class stores and replays
 *    spherical harmonic marker geometry information using a QPainter.
 */
/*======================================================================*/
class NucleusMarkerOrthoViewRendererCache :
    public SHSurfaceMarkerOrthoViewRendererCache
{

public:
  
  NucleusMarkerOrthoViewRendererCache();
  NucleusMarkerOrthoViewRendererCache(
      NucleusMarkerOrthoViewRendererCache const &cache);
  virtual ~NucleusMarkerOrthoViewRendererCache();

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
 *   Set mitotic flag. Calling this function sets the show mitotic flag.
 *
 *   \param mitotic The new mitotic flag value
 */
/*======================================================================*/
  void setMitotic(bool mitotic);
  
/*======================================================================*/
/*! 
 *   Set predictedMitotic flag. Calling this function clears the showMitotic
 *   flag
 *
 *   \param predictedMitotic The new predicted mitotic flag value
 */
/*======================================================================*/
  void setPredictedMitotic(bool predictedMitotic);
  
/*======================================================================*/
/*! 
 *   Set continuity
 *
 *   \param The new continuity 
 */
/*======================================================================*/
  void setContinuity(int continuity);

/*======================================================================*/
/*! 
 *   Set predicted continuity
 *
 *   \param predictedContinuity The new predicted continuity
 */
/*======================================================================*/
  void setPredictedContinuity(int predictedContinuity);

protected:
  
  bool _showMitotic, _mitotic, _predictedMitotic;
  int _continuity, _predictedContinuity;

};


class NucleusMarkerOrthoViewRenderer : public SHSurfaceMarkerOrthoViewRenderer
{

Q_OBJECT

public:
  
  NucleusMarkerOrthoViewRenderer(NucleusMarker* marker, OrthoViewWidget* view);
  NucleusMarkerOrthoViewRenderer(
      NucleusMarkerOrthoViewRenderer const &renderer);
  virtual ~NucleusMarkerOrthoViewRenderer();
  
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
