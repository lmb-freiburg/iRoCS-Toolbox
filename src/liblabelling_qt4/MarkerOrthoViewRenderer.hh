/**************************************************************************
**       Title: Abstract Marker OrthoView Renderer
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    This class is the interface between the OrthoView and different Marker
**    types. For each Marker type one such interface class needs to be
**    implemented.
**
**************************************************************************/

#ifndef MARKERORTHOVIEWRENDERER_HH
#define MARKERORTHOVIEWRENDERER_HH

#include "MarkerRenderer.hh"

#include <blitz/array.h>

#include <libProgressReporter/ProgressReporter.hh>

class OrthoViewWidget;

/*======================================================================*/
/*!
 *  \class MarkerOrthoViewRendererCache MarkerOrthoViewRenderer.hh "MarkerOrthoViewRenderer.hh"
 *  \brief The MarkerOrthoViewRendererCache class specifies the interface
 *    to store and replay marker geometry information using a QPainter.
 *
 *  This class has to be reimplemented by all specialized
 *  MarkerOrthoViewRenderers.
 */
/*======================================================================*/
class MarkerOrthoViewRendererCache
{

public:
  
  MarkerOrthoViewRendererCache();
  MarkerOrthoViewRendererCache(MarkerOrthoViewRendererCache const &cache);
  virtual ~MarkerOrthoViewRendererCache();

/*======================================================================*/
/*! 
 *   Get a new copy of this cache
 *
 *   \return The address to the clone of this cache.
 */
/*======================================================================*/
  virtual MarkerOrthoViewRendererCache *clone() const = 0;

/*======================================================================*/
/*! 
 *   Set the active flag of this cache. If the cache is inactive a call to
 *   the render() method is a noop.
 *
 *   \param active The new active flag value
 */
/*======================================================================*/
  void setActive(bool active);

/*======================================================================*/
/*! 
 *   Get the active flag of this cache.
 *
 *   \return The current active flag
 */
/*======================================================================*/
  bool active() const;

/*======================================================================*/
/*! 
 *   Set the showHelperLines flag of this cache. If this flag is set
 *   additional helper lines for specific marker types are drawn.
 *
 *   \param active The new showHelperLines flag value
 */
/*======================================================================*/
  void setShowHelperLines(bool showHelperLines);

/*======================================================================*/
/*! 
 *   Get the showHelperLines flag of this cache.
 *
 *   \return The current showHelperLines flag
 */
/*======================================================================*/
  bool showHelperLines() const;

/*======================================================================*/
/*! 
 *   Set the font size for text drawn by this cache.
 *
 *   \param fontSize The font size to use for drawing text
 */
/*======================================================================*/
  void setFontSize(int fontSize);

/*======================================================================*/
/*! 
 *   Get the font size for text drawn by this cache.
 *
 *   \return The font size to use for drawing text
 */
/*======================================================================*/
  int fontSize() const;

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
  virtual void render(QPainter *painter) const = 0;

protected:
  
  bool _active, _showHelperLines;
  int _fontSize;

};


class MarkerOrthoViewRenderer : public MarkerRenderer
{

Q_OBJECT

public:
  
  MarkerOrthoViewRenderer(Marker* marker, OrthoViewWidget* view);
  MarkerOrthoViewRenderer(MarkerOrthoViewRenderer const &renderer);
  virtual ~MarkerOrthoViewRenderer();

  virtual MarkerRenderer *clone() const = 0;

/*======================================================================*/
/*! 
 *   Before calling render set the dimension to render using this method.
 *
 *   \param orthogonalDimension The orthogonal dimension to the orthoview
 *     plane to render
 */
/*======================================================================*/
  void setOrthogonalDimension(int orthogonalDimension);

/*======================================================================*/
/*! 
 *   Main rendering routine which is called whenever a paintEvent() on the
 *   Marker occurs. Within this function the actual rendering of the
 *   associated Marker into the associated view is performed. The orthoview
 *   plane the painter is associated to has to be set prior to the call to
 *   the render() method using setOrthogonalDimension().
 *
 *   Within this method the passed QPainter may be used but painting may be
 *   also done by other means as e.g. by directly imposing OpenGL commands.
 *   The default implementation renders the current marker cache for the
 *   selected orthoview plane.
 *
 *   \param painter The active QPainter or NULL if no painter is currently
 *     active or the view update is not QPainter-based, like e.g. for OpenGL
 *     rendering
 */
/*======================================================================*/
  virtual void render(QPainter* painter = NULL) const;

/*======================================================================*/
/*! 
 *   Render this marker into the given SVG-stream. The default implementation
 *   does nothing. Overload this function if you want to be able to render
 *   the view of the given marker type as 2D vector graphics.
 *
 *   \param svgStream  The output stream to append this channel
 *     visualization to
 *   \param shapePx    The shape of the rectangular ROI to render in voxel
 *     coordinates. It is only given for convenience, since one could
 *     compute this information from the model's element size and the given
 *     bounds
 *   \param lowerBoundUm  The lower bounds of the rectangular ROI to render
 *   \param upperBoundUm  The upper bounds of the rectangular POI to render
 *   \param font          The font to use for textual annotations
 *   \param fontSizePt    The font size in pt
 *   \param pr            If given progress will be reported via that
 *     ProgressReporter
 */
/*======================================================================*/
  virtual void render(
      QXmlStreamWriter &svgStream,
      blitz::TinyVector<double,3> const &shapePx,
      blitz::TinyVector<double,3> const &lowerBoundUm,
      blitz::TinyVector<double,3> const &upperBoundUm,
      std::string const &font, double fontSizePt,
      iRoCS::ProgressReporter *pr = NULL) const = 0;

public slots:

/*======================================================================*/
/*! 
 *   Set the font size for text rendering of this Marker
 *
 *   \param fontSize The font size in px
 */
/*======================================================================*/
  void setFontSize(int fontSize)
        {
          for (int i = 0; i < 3; ++i) _cache(i)->setFontSize(fontSize);
        }

/*======================================================================*/
/*! 
 *   Update the rendering cache for all orthoview planes.
 *
 *   This function is called by the associated ChannelSpecs if its content
 *   changes such that an cache update might be neccessary. The default
 *   implementation calls the updateCache(int) for every direction.
 *   You can disable the cache updates using the setCacheUpdatesEnabled()
 *   function. This is important if you programmatically perform many
 *   operations on the ChannelSpecs, of which each would need a cache
 *   update. After the sequence of operations, you have to call updateCache()
 *   explicitely to apply the changes. After cache update update() of the
 *   view is called to visualize the changes. So if many channels are
 *   involved you should call setUpdatesEnabled(false) to the view before
 *   these changes.
 *
 *   Usually this function need not be reimplemented. Reimplement the
 *   dimension-aware version updateCache(int) instead.
 */
/*======================================================================*/
  virtual void updateCache() const;

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

protected:
  
/*======================================================================*/
/*! 
 *   Set the pen according to the PresetWidget
 *
 *   \param painter    The painter to set the pen for
 *   \param hilighted  If the marker is selected set this bool to true
 *
 *   \return true if the marker is visible, false if not
 */
/*======================================================================*/
  bool _setPenColor(QPainter *painter, bool hilighted = false) const;

/*======================================================================*/
/*! 
 *   Get the RGB color string from the Settings in the PresetWidget. It is
 *   of the form #RRGGBB.
 *
 *   \param draw This bool is false if the marker has an alpha value of zero
 *     and is therefore not visible
 *
 *   \return The marker color as RGB color string
 */
/*======================================================================*/
  std::string _getColorString(bool &draw) const;

  int _orthogonalDimension;

  mutable blitz::TinyVector<MarkerOrthoViewRendererCache*,3> _cache;

};

#endif
