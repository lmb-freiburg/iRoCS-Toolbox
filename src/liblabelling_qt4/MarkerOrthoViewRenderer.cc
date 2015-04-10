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

#include "MarkerOrthoViewRenderer.hh"

#include <QtGui/QPainter>

#include "OrthoViewWidget.hh"
#include "OrthoViewPlane.hh"
#include "MultiChannelModel.hh"
#include "AnnotationChannelSpecs.hh"
#include "NucleusMarker.hh"
#include "CellMarker.hh"

MarkerOrthoViewRendererCache::MarkerOrthoViewRendererCache()
        : _active(false), _showHelperLines(false), _fontSize(5)
{}

MarkerOrthoViewRendererCache::MarkerOrthoViewRendererCache(
    MarkerOrthoViewRendererCache const &cache)
        : _active(cache._active), _showHelperLines(cache._showHelperLines),
          _fontSize(cache._fontSize)
{}

MarkerOrthoViewRendererCache::~MarkerOrthoViewRendererCache()
{}

void MarkerOrthoViewRendererCache::setActive(bool active)
{
  _active = active;
}

bool MarkerOrthoViewRendererCache::active() const
{
  return _active;
}

void MarkerOrthoViewRendererCache::setShowHelperLines(bool showHelperLines)
{
  _showHelperLines = showHelperLines;
}

bool MarkerOrthoViewRendererCache::showHelperLines() const
{
  return _showHelperLines;
}

void MarkerOrthoViewRendererCache::setFontSize(int fontSize)
{
  _fontSize = fontSize;
}

int MarkerOrthoViewRendererCache::fontSize() const
{
  return _fontSize;
}

MarkerOrthoViewRenderer::MarkerOrthoViewRenderer(
    Marker* marker, OrthoViewWidget* view)
        : MarkerRenderer(marker, view), _orthogonalDimension(0),
          _cache(reinterpret_cast<MarkerOrthoViewRendererCache*>(NULL))
{}

MarkerOrthoViewRenderer::MarkerOrthoViewRenderer(
   MarkerOrthoViewRenderer const &renderer)
        : MarkerRenderer(renderer),
          _orthogonalDimension(renderer._orthogonalDimension)
{
  for (int d = 0; d < 3; ++d)
  {
    if (renderer._cache(d) != NULL) _cache(d) = renderer._cache(d)->clone();
    else _cache(d) = NULL;
  }
}

MarkerOrthoViewRenderer::~MarkerOrthoViewRenderer()
{
  for (int d = 0; d < 3; ++d) delete _cache(d);
}

void MarkerOrthoViewRenderer::setOrthogonalDimension(
    int orthogonalDimension)
{
  _orthogonalDimension = orthogonalDimension;
}

void MarkerOrthoViewRenderer::render(QPainter* painter) const
{
  if (painter == NULL)
  {
    std::cerr << __FILE__ << ":" << __LINE__
              << ": SurfaceMarkerOrthoViewRenderer::render(): ERROR: "
              << "No QPainter set!" << std::endl;
    return;
  }
  if (!painter->isActive()) return;

  if (_cache(_orthogonalDimension) != NULL &&
      _cache(_orthogonalDimension)->active())
  {
    painter->save();
    if (!_setPenColor(painter, p_marker->selected()))
    {
      painter->restore();
      return;
    }
    _cache(_orthogonalDimension)->render(painter);
    painter->restore();
  }
}

void MarkerOrthoViewRenderer::updateCache() const
{
  if (!cacheUpdatesEnabled()) return;
  p_view->setUpdatesEnabled(false);
  for (int d = 0; d < 3; ++d) updateCache(d);
  p_view->setUpdatesEnabled(true);
  p_view->update();
}

void MarkerOrthoViewRenderer::updateCache(int) const
{
  if (!cacheUpdatesEnabled()) return;
  p_view->update();
}

bool MarkerOrthoViewRenderer::_setPenColor(QPainter *painter, bool hilighted)
    const
{
  if (painter == NULL || !painter->isActive()) return false;
  QPen pen(painter->pen());

  AnnotationChannelSpecs *channel = p_marker->channel();
  switch (channel->selectedViewType())
  {
  case 0 : // Label
  {
    int alpha = static_cast<int>(
        255.0f * channel->alpha() *
        channel->alphaForLabel(p_marker->label()));
    if (alpha == 0) return false;
    blitz::TinyVector<float,3> c(channel->colorForLabel(p_marker->label()));

    int sublabel = -1;
    if (p_marker->inherits(Marker::Nucleus))
        sublabel = static_cast<NucleusMarker*>(p_marker)->subtype();
    else if (p_marker->inherits(Marker::Cell))
        sublabel = static_cast<CellMarker*>(p_marker)->subtype();
    if (sublabel > 0) c += static_cast<float>(sublabel) * 0.2f;
    for (int d = 0; d < 3; ++d) if (c(d) > 1.0f) c(d) = 1.0f;
    
    pen.setColor(QColor(static_cast<int>(255 * c(0)),
                        static_cast<int>(255 * c(1)),
                        static_cast<int>(255 * c(2)), alpha));
    break;
  }
  case 1 : // Predicted Label
  {
    int alpha = static_cast<int>(
        255.0f * channel->alpha() *
        channel->alphaForLabel(p_marker->predictedLabel()));
    if (alpha == 0) return false;
    blitz::TinyVector<float,3> c(
        channel->colorForLabel(p_marker->predictedLabel()));

    int sublabel = -1;
    if (p_marker->inherits(Marker::Nucleus))
        sublabel = static_cast<NucleusMarker*>(p_marker)->predictedSubtype();
    else if (p_marker->inherits(Marker::Cell))
        sublabel = static_cast<CellMarker*>(p_marker)->predictedSubtype();
    if (sublabel > 0) c += static_cast<float>(sublabel) * 0.2f;
    for (int d = 0; d < 3; ++d) if (c(d) > 1.0f) c(d) = 1.0f;
    
    pen.setColor(QColor(static_cast<int>(255 * c(0)),
                        static_cast<int>(255 * c(1)),
                        static_cast<int>(255 * c(2)), alpha));
    break;
  }
  case 2 : // Confusion
  {
    int alpha = static_cast<int>(channel->alpha() * 255.0f);
    if (alpha == 0) return false;
    if (p_marker->manual() && p_marker->predictedLabel() != -1)
    {
      if (p_marker->label() == p_marker->predictedLabel())
          pen.setColor(QColor(0, 255, 0, alpha));
      else pen.setColor(QColor(255, 0, 0, alpha));
    }
    else pen.setColor(QColor(255, 255, 0, alpha));
    break;
  }
  case 3: // Random
  {
    int alpha = static_cast<int>(channel->alpha() * 255.0f);
    if (alpha == 0) return false;
    blitz::TinyVector<int,3> c(p_marker->randomColor() * 255.0f);
    pen.setColor(QColor(c(0), c(1), c(2), alpha));
    break;    
  }
  default:
    std::cerr << __FILE__ << ":" << __LINE__ << ": Missing implementation"
              << std::endl;
    exit(-1);
  }

  if (hilighted)
  {
    pen.setCosmetic(false);
    pen.setWidth(2);
  }
  else pen.setCosmetic(true);

  painter->setPen(pen);
  return true;
}

std::string MarkerOrthoViewRenderer::_getColorString(bool &draw) const
{
  AnnotationChannelSpecs *channel = p_marker->channel();
  std::string markerColor;
  switch (channel->selectedViewType())
  {
  case 0 : // Label
  {
    blitz::TinyVector<float,3> c(channel->colorForLabel(p_marker->label()));

    int sublabel = -1;
    if (p_marker->inherits(Marker::Nucleus))
        sublabel = static_cast<NucleusMarker*>(p_marker)->subtype();
    else if (p_marker->inherits(Marker::Cell))
        sublabel = static_cast<CellMarker*>(p_marker)->subtype();
    if (sublabel > 0) c += static_cast<float>(sublabel) * 0.2f;
    for (int d = 0; d < 3; ++d) if (c(d) > 1.0f) c(d) = 1.0f;
    
    markerColor =
        ("#" +
         QString::number(
             static_cast<int>(255 * c(0)), 16).rightJustified(2, '0', true) +
         QString::number(
             static_cast<int>(255 * c(1)), 16).rightJustified(2, '0', true) +
         QString::number(
             static_cast<int>(255 * c(2)), 16).rightJustified(
                 2, '0', true)).toStdString();
    draw = (channel->alphaForLabel(p_marker->label()) != 0.0f);
    break;
  }
  case 1 : // Predicted Label
  {
    blitz::TinyVector<float,3> c(
        channel->colorForLabel(p_marker->predictedLabel()));

    int sublabel = -1;
    if (p_marker->inherits(Marker::Nucleus))
        sublabel = static_cast<NucleusMarker*>(p_marker)->predictedSubtype();
    else if (p_marker->inherits(Marker::Cell))
        sublabel = static_cast<CellMarker*>(p_marker)->predictedSubtype();
    if (sublabel > 0) c += static_cast<float>(sublabel) * 0.2f;
    for (int d = 0; d < 3; ++d) if (c(d) > 1.0f) c(d) = 1.0f;
    
    markerColor =
        ("#" +
         QString::number(
             static_cast<int>(255 * c(0)), 16).rightJustified(2, '0', true) +
         QString::number(
             static_cast<int>(255 * c(1)), 16).rightJustified(2, '0', true) +
         QString::number(
             static_cast<int>(255 * c(2)), 16).rightJustified(
                 2, '0', true)).toStdString();
    draw = (channel->alphaForLabel(p_marker->predictedLabel()) != 0.0f);
    break;
  }
  case 2 : // Confusion
    if (p_marker->manual() && p_marker->predictedLabel() != -1)
    {
      if (p_marker->label() == p_marker->predictedLabel())
          markerColor = "#00ff00";
      else markerColor = "#ff0000";
    }
    else markerColor = "#ffff00";
    draw = true;
    break;
  case 3 : // Random
    markerColor =
        ("#" + QString::number(
            static_cast<int>(p_marker->randomColor()(0) * 255),
            16).rightJustified(2, '0', true) +
         QString::number(
             static_cast<int>(p_marker->randomColor()(1) * 255),
             16).rightJustified(2, '0', true) +
         QString::number(
             static_cast<int>(p_marker->randomColor()(2) * 255),
             16).rightJustified(2, '0', true)).toStdString();
    draw = true;
    break;
  default:
    std::cerr << __FILE__ << ":" << __LINE__ << ": Missing implementation"
              << std::endl;
    exit(-1);
  }
  return markerColor;
}

