/**************************************************************************
**       Title: Abstract Marker-View Renderer
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    This class is the interface between the different View- and Marker
**    types. For each Marker type and each View type one s**    class needs to be implemented.
**
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

