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

#ifndef ANNOTATIONCHANNELSPECSORTHOVIEWRENDERER_HH
#define ANNOTATIONCHANNELSPECSORTHOVIEWRENDERER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "ChannelSpecsOrthoViewRenderer.hh"

class AnnotationChannelSpecs;
class OrthoViewWidget;
class Marker;

class AnnotationChannelSpecsOrthoViewRenderer :
    public ChannelSpecsOrthoViewRenderer
{

Q_OBJECT

public:

  AnnotationChannelSpecsOrthoViewRenderer(
      AnnotationChannelSpecs* channel, OrthoViewWidget* view);
  ~AnnotationChannelSpecsOrthoViewRenderer();

  void userInteractionEvent(UserInteractionEvent* event) override;
  void render(QPainter* painter = NULL) const override;
  void render(
      QXmlStreamWriter &svgStream,
      blitz::TinyVector<double,3> const &shapePx,
      blitz::TinyVector<double,3> const &lowerBoundUm,
      blitz::TinyVector<double,3> const &upperBoundUm,
      std::string const &font, double fontSizePt,
      iRoCS::ProgressReporter *pr = NULL) const override;

public slots:

/*======================================================================*/
/*!
 *   Enables or disables the updates of the rendering cache. This usually
 *   means that changes in the ChannelSpecs or the view position don't
 *   lead to an update of the view. This is useful, if you have to
 *   programmatically perform many small operations on the channel of which
 *   not each should trigger a repaint.
 *
 *   \param enable true to enable, false to disable cache updates
 */
/*======================================================================*/
  virtual void setCacheUpdatesEnabled(bool enable) override;

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
  virtual void updateCache(int direction) const override;

private:

  Marker *p_marker;

};

#endif
