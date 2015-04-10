/**************************************************************************
 *
 * This file belongs to the iRoCS Toolbox.
 *
 * Copyright (C) 2015 Thorsten Falk
 *
 *        Image Analysis Lab, University of Freiburg, Germany
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 **************************************************************************/

#ifndef NUCLEUSMARKERORTHOVIEWRENDERER_HH
#define NUCLEUSMARKERORTHOVIEWRENDERER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

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
 *   \param continuity The new continuity 
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
