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

#include "MarkerRenderer.hh"

#include "UserInteractionEvent.hh"

#include "ViewWidget.hh"
#include "Marker.hh"

MarkerRenderer::MarkerRenderer(Marker* marker, ViewWidget* view)
        : QObject(), p_marker(marker), p_view(view), _cacheUpdatesEnabled(true)
{
  if (p_marker != NULL) _cacheUpdatesEnabled = p_marker->updatesEnabled();
}

MarkerRenderer::MarkerRenderer(MarkerRenderer const &renderer)
        : QObject(), p_marker(renderer.p_marker), p_view(renderer.p_view),
          _cacheUpdatesEnabled(renderer._cacheUpdatesEnabled)
{}

MarkerRenderer::~MarkerRenderer()
{}

Marker* MarkerRenderer::marker() const
{
  return p_marker;
}

ViewWidget* MarkerRenderer::view() const
{
  return p_view;
}

bool MarkerRenderer::isDragging() const
{
  return _dragging;
}

void MarkerRenderer::setDragging(bool drag)
{
  _dragging = drag;
}

bool MarkerRenderer::cacheUpdatesEnabled() const
{
  return _cacheUpdatesEnabled;
}

void MarkerRenderer::userInteractionEvent(UserInteractionEvent *event)
{
  event->ignore();
}

void MarkerRenderer::setCacheUpdatesEnabled(bool enable)
{
  _cacheUpdatesEnabled = enable;
}

void MarkerRenderer::updateCache() const
{
  if (!_cacheUpdatesEnabled) return;
  p_view->update();
}

