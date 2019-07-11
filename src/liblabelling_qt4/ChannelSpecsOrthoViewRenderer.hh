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

#ifndef CHANNELSPECSORTHOVIEWRENDERER_HH
#define CHANNELSPECSORTHOVIEWRENDERER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "ChannelSpecsRenderer.hh"

#include <blitz/array.h>

#include <libProgressReporter/ProgressReporter.hh>

class OrthoViewWidget;

class ChannelSpecsOrthoViewRenderer : public ChannelSpecsRenderer
{

Q_OBJECT

public:

  ChannelSpecsOrthoViewRenderer(ChannelSpecs* channel, OrthoViewWidget* view);
  virtual ~ChannelSpecsOrthoViewRenderer();

  void setOrthogonalDimension(int orthogonalDimension);

  using ChannelSpecsRenderer::render;

public slots:

/*======================================================================*/
/*!
 *   Update the rendering cache. This function is called by the
 *   associated ChannelSpecs if its content changes such that an cache
 *   update might be neccessary. The default implementation calls the
 *   updateCache(int) for every direction . You can disable the cache
 *   updates using the setCacheUpdatesEnabled() function. This is
 *   important if you programmatically perform many operations on the
 *   ChannelSpecs, of which each would need a cache update. After the
 *   sequence of operations, you have to call updateCache() explicitely to
 *   apply the changes. After cache update update() of the view is called
 *   to visualize the changes. So if many channels are involved you should
 *   call setUpdatesEnabled(false) to the view before these changes.
 *
 *   Usually this function need not be reimplemented. Reimplement the
 *   dimension-aware version updateCache(int) instead.
 */
/*======================================================================*/
  virtual void updateCache() const override;

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
 *   programmatically perform many operations on the ChannelSpecs, of which
 *   each would need a cache update. After the sequence of operations, you
 *   have to call updateCache() explicitely to apply the changes. After cache
 *   update update() of the view is called to visualize the changes. So if
 *   many channels are involved you should call setUpdatesEnabled(false)
 *   to the view before these changes.
 */
/*======================================================================*/
  virtual void updateCache(int direction) const;

/*======================================================================*/
/*!
 *   Render this channel into the given SVG-stream. The default implementation
 *   does nothing. Overload this function if you want to be able to render
 *   the view of the given channel type as 2D vector graphics.
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
 *   \param pr            If given progress is reported via that
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

signals:

/*======================================================================*/
/*!
 *   This signal must be emitted at the end of a cache update to inform the
 *   view, that the cache for the orthoview plane has new data.
 *
 *   \param source A pointer to this ChannelSpecsRenderer
 *   \param direction The OrthoViewPlane index (its normal direction)
 */
/*======================================================================*/
  void cacheUpdated(
      ChannelSpecsOrthoViewRenderer const *source, int direction) const;

protected:

  int _orthogonalDimension;

};

#endif
