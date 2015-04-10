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

#include "CellMarkerOrthoViewRenderer.hh"

#include <QtGui/QPainter>
#include <QtXml/QXmlStreamWriter>

#include "CellMarker.hh"
#include "OrthoViewWidget.hh"
#include "OrthoViewPlane.hh"
#include "AnnotationChannelSpecs.hh"

#if defined HAVE_CONFIG_H && !defined PACKAGE
#include <config.hh>
#endif

#ifdef HAVE_BLITZ_V9
#include <blitz/tinyvec-et.h>
#endif

CellMarkerOrthoViewRendererCache::CellMarkerOrthoViewRendererCache()
        : SurfaceMarkerOrthoViewRendererCache(),
          _continuity(-1), _predictedContinuity(-1)
{}

CellMarkerOrthoViewRendererCache::CellMarkerOrthoViewRendererCache(
    CellMarkerOrthoViewRendererCache const &cache)
        : SurfaceMarkerOrthoViewRendererCache(cache),
          _continuity(cache._continuity),
          _predictedContinuity(cache._predictedContinuity)
{}

CellMarkerOrthoViewRendererCache::~CellMarkerOrthoViewRendererCache()
{}

MarkerOrthoViewRendererCache *CellMarkerOrthoViewRendererCache::clone()
    const
{
  return new CellMarkerOrthoViewRendererCache(*this);
}

void CellMarkerOrthoViewRendererCache::render(QPainter *painter) const
{
  if (painter == NULL || !painter->isActive()) return;
  painter->save();
  painter->setFont(QFont("Helvetica", _fontSize));
  if (_continuity >= 0)
      painter->drawText(QPointF(_point.x() + 3.0f, _point.y() - 3.0f),
                        QString::number(_continuity));
  if (_predictedContinuity >= 0)
      painter->drawText(
          QPointF(_point.x() + 3.0f, _point.y() + (_fontSize + 3.0f)),
          QString::number(_predictedContinuity));
  painter->restore();

  SurfaceMarkerOrthoViewRendererCache::render(painter);
}

int CellMarkerOrthoViewRendererCache::continuity() const
{
  return _continuity;
}

void CellMarkerOrthoViewRendererCache::setContinuity(int continuity)
{
  _continuity = continuity;
}

int CellMarkerOrthoViewRendererCache::predictedContinuity() const
{
  return _predictedContinuity;
}

void CellMarkerOrthoViewRendererCache::setPredictedContinuity(int continuity)
{
  _predictedContinuity = continuity;
}


CellMarkerOrthoViewRenderer::CellMarkerOrthoViewRenderer(
    CellMarker* marker, OrthoViewWidget* view)
        : SurfaceMarkerOrthoViewRenderer(marker, view)
{}

CellMarkerOrthoViewRenderer::CellMarkerOrthoViewRenderer(
    CellMarkerOrthoViewRenderer const &renderer)
        : SurfaceMarkerOrthoViewRenderer(renderer)
{}

CellMarkerOrthoViewRenderer::~CellMarkerOrthoViewRenderer()
{}
  
MarkerRenderer *CellMarkerOrthoViewRenderer::clone() const
{
  return new CellMarkerOrthoViewRenderer(*this);
}

void CellMarkerOrthoViewRenderer::updateCache(int direction) const
{
  if (!cacheUpdatesEnabled()) return;
  
  CellMarker* marker = static_cast<CellMarker*>(p_marker);
  AnnotationChannelSpecs* channel = marker->channel();
  if (channel == NULL)
  {
    std::cerr << __FILE__ << ":" << __LINE__
              << ": Assertion 'marker->channel() != NULL' failed while "
              << "updating the marker cache. A cache update was requested "
              << "for a Marker which is not associated to any channel. "
              << "Update aborted!" << std::endl;
    return;
  }
  MultiChannelModel* model = channel->model();
  if (model == NULL)
  {
    std::cerr << __FILE__ << ":" << __LINE__
              << ": Assertion 'marker->channel()->model() != NULL' failed "
              << "while updating the marker cache. A cache update was "
              << "requested for a Marker whose channel is not associated to "
              << "any MultiChannelModel. Update aborted!" << std::endl;
    return;
  }

  if (_cache(direction) == NULL)
      _cache(direction) = new CellMarkerOrthoViewRendererCache();

  bool visible;
  _getColorString(visible);
  if (!visible)
  {
    _cache(direction)->setActive(false);
    return;
  }

  // Marker is not visible ==> nothing to draw
  if (marker->boundingBoxLowerBoundUm()(direction) >
      p_view->positionUm()(direction) ||
      marker->boundingBoxUpperBoundUm()(direction) <
      p_view->positionUm()(direction))
  {
    _cache(direction)->setActive(false);
    return;
  }

  CellMarkerOrthoViewRendererCache *cache =
      static_cast<CellMarkerOrthoViewRendererCache*>(_cache(direction));

  cache->setContinuity(marker->continuity());
  cache->setPredictedContinuity(marker->predictedContinuity());
  cache->setFontSize(static_cast<OrthoViewWidget*>(p_view)->fontSize());
  
  SurfaceMarkerOrthoViewRenderer::updateCache(direction);
}

void CellMarkerOrthoViewRenderer::render(
    QXmlStreamWriter &svgStream,
    blitz::TinyVector<double,3> const &shapePx,
    blitz::TinyVector<double,3> const &lowerBoundUm,
    blitz::TinyVector<double,3> const &upperBoundUm,
    std::string const &font, double /*fontSizePt*/,
    iRoCS::ProgressReporter *pr) const
{
  if (p_marker->channel() == NULL)
  {
    std::cerr << "CellMarkerOrthoViewRenderer::render(): ERROR: Marker at "
              << "position " << p_marker->positionUm() << " has no asociated "
              << "channel!" << std::endl;
    return;
  }

  if (pr != NULL && !pr->updateProgress(pr->taskProgressMin())) return;

  OrthoViewWidget* view = static_cast<OrthoViewWidget*>(p_view);
  CellMarker* marker = static_cast<CellMarker*>(p_marker);

  double um2PxFactor = shapePx(2) / (upperBoundUm(2) - lowerBoundUm(2));

  // Get dimensions of current plane
  blitz::TinyVector<int,2> const &dimensions(
      view->orthoViewPlane(_orthogonalDimension)->dimensions());

  std::string viewId = view->orthoViewPlane(_orthogonalDimension)->viewId();
  blitz::TinyVector<double,3> viewPosUm(view->positionUm());

  // Get marker position in px
  blitz::TinyVector<float,3> markerPosUm(marker->positionUm());
  blitz::TinyVector<float,3> markerPosPx(markerPosUm * um2PxFactor);
  blitz::TinyVector<float,3> posPx((markerPosUm - lowerBoundUm) * um2PxFactor);

  // Get slice position in px
  double slicePx = viewPosUm(_orthogonalDimension) * um2PxFactor;

  // Get marker extent in slice direction
  double lbPx = marker->boundingBoxLowerBoundUm()(_orthogonalDimension) *
      um2PxFactor;
  double ubPx = marker->boundingBoxUpperBoundUm()(_orthogonalDimension) *
      um2PxFactor;
  
  // Check whether marker is visible, if not: skip rendering
  if (slicePx < lbPx || slicePx > ubPx) return;

  // Set marker color according to view mode
  bool draw;
  std::string markerColor = _getColorString(draw);
  if (!draw) return;

  svgStream.writeStartElement("g");
  svgStream.writeAttribute("id", (viewId + ": " + marker->id()).c_str());

  /*-----------------------------------------------------------------------
   *  draw polygons
   *-----------------------------------------------------------------------*/

  // For each triangle check for intersection with current slice
  typedef std::pair<QPointF,QPointF> SegmentT;
  std::list<SegmentT> segments;
  std::vector<atb::SurfaceGeometry::VertexT> const &vertices =
      marker->vertices();
  std::vector<atb::SurfaceGeometry::IndexT> const &indices =
      marker->indices();
  
  QPointF p[2];
  for (size_t i = 0; i < marker->indices().size(); i += 3)
  {
    blitz::TinyVector<blitz::TinyVector<double,3>,3> tri(
        vertices[indices[i]] + markerPosUm,
        vertices[indices[i + 1]] + markerPosUm,
        vertices[indices[i + 2]] + markerPosUm);
    
    int nPoints = 0;
    for (int edge = 0; edge < 3; ++edge)
    {
      blitz::TinyVector<double,2> pos(
          intersectLineNPlane(
              tri(edge), tri((edge + 1) % 3), _orthogonalDimension,
              viewPosUm(_orthogonalDimension)));
      
      if (blitz::all(pos == std::numeric_limits<double>::infinity()))
          continue;
      
      if (pos(0) == 0.0 && pos(1) ==
          std::numeric_limits<double>::infinity())
      {
        p[0].setX((tri(edge)(dimensions(1)) -
                   lowerBoundUm(dimensions(1))) * um2PxFactor);
        p[0].setY((tri(edge)(dimensions(0)) -
                   lowerBoundUm(dimensions(0))) * um2PxFactor);
        p[1].setX((tri((edge + 1) % 3)(dimensions(1)) -
                   lowerBoundUm(dimensions(1))) * um2PxFactor);
        p[1].setY((tri((edge + 1) % 3)(dimensions(0)) -
                   lowerBoundUm(dimensions(0))) * um2PxFactor);
        nPoints = 2;
        break;
      }
      
      p[nPoints].setX((pos(1) - lowerBoundUm(dimensions(1))) *
                      um2PxFactor);
      p[nPoints].setY((pos(0) - lowerBoundUm(dimensions(0))) *
                      um2PxFactor);
      if (nPoints > 0 && p[nPoints].rx() == p[0].rx() &&
          p[nPoints].ry() == p[0].ry()) continue;
      nPoints++;
    }
    if (nPoints == 2) segments.push_back(SegmentT(p[0], p[1]));
  }

  if (segments.size() == 0) return;

  svgStream.writeStartElement("g");
  svgStream.writeAttribute(
      "id", (viewId + "-" + marker->id() + ": polygon").c_str());
  
  // Try to combine segments to longer paths
  typedef std::list<QPointF> PathT;
  std::list<PathT> paths;
  paths.push_back(PathT());
  paths.back().push_back(segments.front().first);
  paths.back().push_back(segments.front().second);
  segments.pop_front();
  
  while (segments.size() != 0)
  {
    // Try to elongate to the left
    std::list<SegmentT>::iterator it = segments.begin();
    for (; it != segments.end() &&
             !(it->first.x() == paths.back().front().x() &&
               it->first.y() == paths.back().front().y()) &&
             !(it->first.x() == paths.back().back().x() &&
               it->first.y() == paths.back().back().y()) &&
             !(it->second.x() == paths.back().front().x() &&
               it->second.y() == paths.back().front().y()) &&
             !(it->second.x() == paths.back().back().x() &&
               it->second.y() == paths.back().back().y()); ++it);
    if (it != segments.end())
    {
      // Add segment to path
      if (it->first.x() == paths.back().front().x() &&
          it->first.y() == paths.back().front().y())
          paths.back().push_front(it->second);
      else if (it->first.x() == paths.back().back().x() &&
               it->first.y() == paths.back().back().y())
          paths.back().push_back(it->second);
      else if (it->second.x() == paths.back().front().x() &&
               it->second.y() == paths.back().front().y())
          paths.back().push_front(it->first);
      else paths.back().push_back(it->first);
      segments.erase(it);
    }
    else
    {
      // No more segments could be added to current path, if there
      // are segments left create new path
      if (segments.size() != 0)
      {
        paths.push_back(PathT());
        paths.back().push_back(segments.front().first);
        paths.back().push_back(segments.front().second);
        segments.pop_front();
      }
    }
  }

  for (std::list<PathT>::const_iterator it = paths.begin();
       it != paths.end(); ++it)
  {
    QString pathSpec("M" +
                     QString::number(it->front().x()) + " " +
                     QString::number(it->front().y()));
    for (PathT::const_iterator itP = (++it->begin()); itP != it->end(); ++itP)
        pathSpec += " L" + QString::number(itP->x()) + " " +
            QString::number(itP->y());
    svgStream.writeEmptyElement("path");
    svgStream.writeAttribute("d", pathSpec);
    svgStream.writeAttribute(
        "style", ("stroke:" + markerColor +
                  ";fill:none;stroke-width:0.5pt").c_str());
  }

  svgStream.writeEndElement(); // g id="polygon"

  if (marker->continuity() != -1)
  {
    svgStream.writeStartElement("text");
    svgStream.writeAttribute(
        "x", QString::number(posPx(dimensions(1)) + 3.0));
    svgStream.writeAttribute(
        "y", QString::number(posPx(dimensions(0)) - 3.0));
    svgStream.writeAttribute(
        "style", "font-size:" + QString::number(view->fontSize()) +
        "px;font-family:" + font.c_str() + ";stroke:none;fill:" +
        markerColor.c_str());
    svgStream.writeCharacters(QString::number(marker->continuity()));
    svgStream.writeEndElement(); // text
  }
      
  if (marker->predictedContinuity() != -1)
  {
    svgStream.writeStartElement("text");
    svgStream.writeAttribute(
        "x", QString::number(posPx(dimensions(1)) + 3.0));
    svgStream.writeAttribute(
        "y", QString::number(posPx(dimensions(0)) + view->fontSize() + 3.0));
    svgStream.writeAttribute(
        "style", "font-size:" + QString::number(view->fontSize()) +
        "px;font-family:" + font.c_str() + ";stroke:none;fill:" +
        markerColor.c_str() + ";text-anchor:end");
    svgStream.writeCharacters(QString::number(marker->predictedContinuity()));
    svgStream.writeEndElement(); // text
  }
      
  svgStream.writeEndElement(); // g id=<markername>

  if (pr != NULL) pr->updateProgress(pr->taskProgressMax());
}
