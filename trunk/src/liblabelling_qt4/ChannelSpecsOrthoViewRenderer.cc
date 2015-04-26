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

#include "ChannelSpecsOrthoViewRenderer.hh"

#include "OrthoViewWidget.hh"
#include "OrthoViewPlane.hh"
#include "MarkerOrthoViewRenderer.hh"
#include "AnnotationChannelSpecs.hh"

ChannelSpecsOrthoViewRenderer::ChannelSpecsOrthoViewRenderer(
    ChannelSpecs* channel, OrthoViewWidget* view)
        : ChannelSpecsRenderer(channel, view),
          _orthogonalDimension(0)
{
  if (view != NULL)
  {
    for (int d = 0; d < 3; ++d) view->orthoViewPlane(d)->adjustSize();
    connect(
        this, SIGNAL(cacheUpdated(ChannelSpecsOrthoViewRenderer const*,int)),
        view, SLOT(updatePlaneData(ChannelSpecsOrthoViewRenderer const*,int)));
  }
}

ChannelSpecsOrthoViewRenderer::~ChannelSpecsOrthoViewRenderer()
{}

void ChannelSpecsOrthoViewRenderer::setOrthogonalDimension(
    int orthogonalDimension)
{
  _orthogonalDimension = orthogonalDimension;
  if (p_channel->channelType() == ChannelSpecs::Annotation)
  {
    AnnotationChannelSpecs *channel =
        static_cast<AnnotationChannelSpecs*>(p_channel);
    for (std::vector<Marker*>::iterator it = channel->begin();
         it != channel->end(); ++it)
        static_cast<MarkerOrthoViewRenderer*>(
            (*it)->renderer(p_view))->setOrthogonalDimension(
                orthogonalDimension);
  }
}

void ChannelSpecsOrthoViewRenderer::updateCache() const
{
  if (!cacheUpdatesEnabled()) return;
  p_view->setUpdatesEnabled(false);
  for (int d = 0; d < 3; ++d) updateCache(d);
  p_view->setUpdatesEnabled(true);
  p_view->update();
}

void ChannelSpecsOrthoViewRenderer::updateCache(int) const
{
  if (!cacheUpdatesEnabled()) return;
  p_view->update();
}
