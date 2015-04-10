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

#include "SurfaceMarkerOrthoViewRenderer.hh"

#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>
#include <QtXml/QXmlStreamWriter>

#include "MultiChannelModel.hh"
#include "AnnotationChannelSpecs.hh"

#include "SurfaceMarker.hh"
#include "OrthoViewWidget.hh"
#include "OrthoViewPlane.hh"

#include "OrthoViewUserInteractionEvent.hh"


SurfaceMarkerOrthoViewRendererCache::SurfaceMarkerOrthoViewRendererCache()
        : PointMarkerOrthoViewRendererCache(), _lines()
{}

SurfaceMarkerOrthoViewRendererCache::SurfaceMarkerOrthoViewRendererCache(
    SurfaceMarkerOrthoViewRendererCache const &cache)
        : PointMarkerOrthoViewRendererCache(cache), _lines(cache._lines)
{}

SurfaceMarkerOrthoViewRendererCache::~SurfaceMarkerOrthoViewRendererCache()
{}

MarkerOrthoViewRendererCache *SurfaceMarkerOrthoViewRendererCache::clone()
    const
{
  return new SurfaceMarkerOrthoViewRendererCache(*this);
}

void SurfaceMarkerOrthoViewRendererCache::render(
    QPainter *painter) const
{
  if (painter == NULL || !painter->isActive()) return;
  if (_lines.size() != 0) 
  {
    for (size_t i = 0; i < _lines.size(); ++i)
        painter->drawLine(_lines[i](0), _lines[i](1));
  }
  else PointMarkerOrthoViewRendererCache::render(painter);
}

std::vector< blitz::TinyVector<QPointF,2> > const
&SurfaceMarkerOrthoViewRendererCache::lines() const
{
  return _lines;
}

std::vector< blitz::TinyVector<QPointF,2> >
&SurfaceMarkerOrthoViewRendererCache::lines()
{
  return _lines;
}


SurfaceMarkerOrthoViewRenderer::SurfaceMarkerOrthoViewRenderer(
    SurfaceMarker* marker, OrthoViewWidget* view)
        : PointMarkerOrthoViewRenderer(marker, view)
{}

SurfaceMarkerOrthoViewRenderer::SurfaceMarkerOrthoViewRenderer(
    SurfaceMarkerOrthoViewRenderer const &renderer)
        : PointMarkerOrthoViewRenderer(renderer)
{}

SurfaceMarkerOrthoViewRenderer::~SurfaceMarkerOrthoViewRenderer()
{}
  
MarkerRenderer *SurfaceMarkerOrthoViewRenderer::clone() const
{
  return new SurfaceMarkerOrthoViewRenderer(*this);
}

blitz::TinyVector<double,2>
SurfaceMarkerOrthoViewRenderer::intersectLineNPlane(
    const blitz::TinyVector<double,3>& p1,
    const blitz::TinyVector<double,3>& p2,
    int sliceDim, double slicePosition)
{
  double inf = std::numeric_limits<double>::infinity();

  // No intersection
  if ((p1(sliceDim) < slicePosition && p2(sliceDim) < slicePosition) ||
      (p1(sliceDim) > slicePosition && p2(sliceDim) > slicePosition))
      return blitz::TinyVector<double,2>(inf, inf);

  double alpha = -1;
  // Parallel or in-plane
  if (p1(sliceDim) == p2(sliceDim))
  {
    if (p1(sliceDim) == slicePosition)
        return blitz::TinyVector<double,2>(0.0, inf);
    else return blitz::TinyVector<double,2>(inf, inf);
  }
  else alpha = (slicePosition - p1(sliceDim)) / (p2(sliceDim) - p1(sliceDim));

  // Line has no intersection with plane
  //if (alpha < 0 || alpha > 1) return blitz::TinyVector<double,2>(inf, inf);

  blitz::TinyVector<double,2> res(0.0);
  switch (sliceDim)
  {
  case 0 :
    res(0) = p1(1) + alpha * (p2(1) - p1(1));
    res(1) = p1(2) + alpha * (p2(2) - p1(2));
    break;
  case 1 :
    res(0) = p1(0) + alpha * (p2(0) - p1(0));
    res(1) = p1(2) + alpha * (p2(2) - p1(2));
    break;
  case 2 :
    res(0) = p1(1) + alpha * (p2(1) - p1(1));
    res(1) = p1(0) + alpha * (p2(0) - p1(0));
    break;
  }
  return res;
}

void SurfaceMarkerOrthoViewRenderer::render(
    QXmlStreamWriter &svgStream,
    blitz::TinyVector<double,3> const &shapePx,
    blitz::TinyVector<double,3> const &lowerBoundUm,
    blitz::TinyVector<double,3> const &upperBoundUm,
    std::string const &/*font*/, double /*fontSize*/,
    iRoCS::ProgressReporter *pr) const
{
  if (p_marker->channel() == NULL)
  {
    std::cerr << "SurfaceMarkerOrthoViewRenderer::render(): ERROR: Marker at "
              << "position " << p_marker->positionUm() << " has no asociated "
              << "channel!" << std::endl;
    return;
  }

  if (pr != NULL && !pr->updateProgress(pr->taskProgressMin())) return;

  OrthoViewWidget* view = static_cast<OrthoViewWidget*>(p_view);
  SurfaceMarker* marker = static_cast<SurfaceMarker*>(p_marker);

  double um2PxFactor = shapePx(2) / (upperBoundUm(2) - lowerBoundUm(2));

  // Get dimensions of current plane
  blitz::TinyVector<int,2> const &dimensions(
      view->orthoViewPlane(_orthogonalDimension)->dimensions());

  std::string viewId = view->orthoViewPlane(_orthogonalDimension)->viewId();
  blitz::TinyVector<double,3> viewPosUm(view->positionUm());

  // Get marker position in px
  blitz::TinyVector<double,3> markerPosUm(marker->positionUm());
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

  typedef std::list<QPointF> PathT;
  std::list<PathT> paths;
  paths.push_back(PathT());
  paths.back().push_back(segments.front().first);
  paths.back().push_back(segments.front().second);
  segments.pop_front();
  
  // std::cerr << "nSegments = " << segments.size() << std::flush;

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

  // std::cerr << " - combined to " << paths.size() << " paths" << std::endl;

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
  svgStream.writeEndElement(); // g id=<markername>

  if (pr != NULL) pr->updateProgress(pr->taskProgressMax());
}

void SurfaceMarkerOrthoViewRenderer::userInteractionEvent(
    UserInteractionEvent *event)
{
  OrthoViewUserInteractionEvent* e =
      reinterpret_cast<OrthoViewUserInteractionEvent*>(event);
  e->ignore();
 
  if (e->mouseEvent() != NULL &&
      e->mouseEvent()->type() == QEvent::MouseButtonRelease &&
      p_marker->channel()->selectedOperation() ==
      AnnotationChannelSpecs::LabelMarker)
  {
    if (e->mouseEvent()->button() == Qt::LeftButton)
    {
      p_marker->setManual(true);
      p_marker->channel()->labelMarker(p_marker);
      e->accept();
      return;
    }
    
    if (e->mouseEvent()->button() == Qt::RightButton)
    {
      p_marker->setManual(false);
      p_marker->setLabel(0);
      e->accept();
      return;
    }
  }
}

void SurfaceMarkerOrthoViewRenderer::updateCache(int direction) const
{
  if (!cacheUpdatesEnabled()) return;
  
  OrthoViewWidget* view = static_cast<OrthoViewWidget*>(p_view);
  SurfaceMarker* marker = static_cast<SurfaceMarker*>(p_marker);
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
      _cache(direction) = new SurfaceMarkerOrthoViewRendererCache();

  SurfaceMarkerOrthoViewRendererCache *cache =
      static_cast<SurfaceMarkerOrthoViewRendererCache*>(_cache(direction));

  bool visible;
  _getColorString(visible);
  if (!visible)
  {
    cache->setActive(false);
    return;
  }

  if (marker->indices().size() == 0)
  {
    PointMarkerOrthoViewRenderer::updateCache(direction);
    return;
  }
  
  blitz::TinyVector<double,3> viewPosUm(p_view->positionUm());
  double viewPosUmInDirection = viewPosUm(direction);

  // Marker is not visible ==> nothing to draw
  if (marker->boundingBoxLowerBoundUm()(direction) > viewPosUmInDirection ||
      marker->boundingBoxUpperBoundUm()(direction) < viewPosUmInDirection)
  {
    cache->setActive(false);
    return;
  }

  // Get dimensions of current plane
  blitz::TinyVector<int,2> const &dims(
      view->orthoViewPlane(direction)->dimensions());
  
  // Get marker position in Pixel coordinates
  blitz::TinyVector<double,3> markerPosUm(marker->positionUm());
  blitz::TinyVector<double,3> markerPosPx(view->um2Px(markerPosUm));

  // Cross Offset in micrometers
  double sliceOffsetUm =
      std::abs(viewPosUm(direction) - markerPosUm(direction));

  // Get the corresponding screen (in-Plane) coordinates
  QPointF posPx = QPointF(markerPosPx(dims(1)), markerPosPx(dims(0)));

  cache->lines().clear();

  /*-----------------------------------------------------------------------
   *  draw polygons
   *-----------------------------------------------------------------------*/
  bool oldStyle = true; // New style should theoretically work, but somehow...
  if (oldStyle)
  {
    std::vector<atb::SurfaceGeometry::VertexT> const &vertices =
        marker->vertices();
    std::vector<atb::SurfaceGeometry::IndexT> const &indices =
        marker->indices();
    
    // For each triangle check for intersection with current slice
    QPointF p[2];
    for (size_t t = 0; t < indices.size(); t += 3)
    {
      blitz::TinyVector<blitz::TinyVector<double,3>,3> tri(
          vertices[indices[t]] + markerPosUm,
          vertices[indices[t + 1]] + markerPosUm,
          vertices[indices[t + 2]] + markerPosUm);
      
      int nPoints = 0;
      for (int edge = 0; edge < 3; ++edge)
      {
        blitz::TinyVector<double,2> pos(
            intersectLineNPlane(
                tri(edge), tri((edge + 1) % 3), direction,
                viewPosUmInDirection));
        
        if (blitz::all(pos == std::numeric_limits<double>::infinity()))
            continue;
        
        if (pos(0) == 0.0 && pos(1) == std::numeric_limits<double>::infinity())
        {
          p[0].setX(view->um2Px(tri(edge)(dims(1)), dims(1)));
          p[0].setY(view->um2Px(tri(edge)(dims(0)), dims(0)));
          p[1].setX(view->um2Px(tri((edge + 1) % 3)(dims(1)), dims(1)));
          p[1].setY(view->um2Px(tri((edge + 1) % 3)(dims(0)), dims(0)));
          nPoints = 2;
          break;
        }
        p[nPoints].setX(view->um2Px(pos(1), dims(1)));
        p[nPoints].setY(view->um2Px(pos(0), dims(0)));
        if (nPoints > 0 && p[nPoints].rx() == p[0].rx() &&
            p[nPoints].ry() == p[0].ry()) continue;
        nPoints++;
      }
      if (nPoints == 2)
          cache->lines().push_back(blitz::TinyVector<QPointF,2>(p[0], p[1]));
    }
  }
  else
  {
    std::vector< blitz::TinyVector<blitz::TinyVector<float,2>,2> > lines;
    marker->surface().planeIntersection(direction, sliceOffsetUm, lines);
    for (size_t i = 0; i < lines.size(); ++i)
    {
      cache->lines().push_back(
          blitz::TinyVector<QPointF,2>(
              QPointF(view->um2Px(markerPosUm(dims(1)) + lines[i](0)(1),
                                  dims(1)),
                      view->um2Px(markerPosUm(dims(0)) + lines[i](0)(0),
                                  dims(0))),
              QPointF(view->um2Px(markerPosUm(dims(1)) + lines[i](1)(1),
                                  dims(1)),
                      view->um2Px(markerPosUm(dims(0)) + lines[i](1)(0),
                                  dims(0)))));
    }
  }
  
  cache->setPoint(posPx);

  cache->setActive(true);
  if (thread() == qApp->thread()) view->update();
}
