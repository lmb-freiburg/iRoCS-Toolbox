/**************************************************************************
**       Title: NucleusMarker-OrthoView Renderer
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    This class is the interface between the different View- and Marker
**    types. For each Marker type and each View type one such interface
**    class needs to be implemented. This is the specific implementation
**    for NucleusMarker <=> OrthoView
**
**************************************************************************/

#include "NucleusMarkerOrthoViewRenderer.hh"

#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>
#include <QtXml/QXmlStreamWriter>

#include "MultiChannelModel.hh"
#include "AnnotationChannelSpecs.hh"

#include "NucleusMarker.hh"
#include "OrthoViewWidget.hh"
#include "OrthoViewPlane.hh"

#include "OrthoViewUserInteractionEvent.hh"


NucleusMarkerOrthoViewRendererCache::NucleusMarkerOrthoViewRendererCache()
        : SHSurfaceMarkerOrthoViewRendererCache(), _showMitotic(true),
          _mitotic(false), _predictedMitotic(false),
          _continuity(-1), _predictedContinuity(-1)
{}

NucleusMarkerOrthoViewRendererCache::NucleusMarkerOrthoViewRendererCache(
    NucleusMarkerOrthoViewRendererCache const &cache)
        : SHSurfaceMarkerOrthoViewRendererCache(cache),
          _showMitotic(cache._mitotic), _mitotic(cache._mitotic),
          _predictedMitotic(cache._predictedMitotic),
          _continuity(cache._continuity),
          _predictedContinuity(cache._predictedContinuity)
{}

NucleusMarkerOrthoViewRendererCache::~NucleusMarkerOrthoViewRendererCache()
{}

MarkerOrthoViewRendererCache *NucleusMarkerOrthoViewRendererCache::clone()
    const
{
  return new NucleusMarkerOrthoViewRendererCache(*this);
}

void NucleusMarkerOrthoViewRendererCache::setMitotic(bool mitotic)
{
  _mitotic = mitotic;
  _showMitotic = true;
}

void NucleusMarkerOrthoViewRendererCache::setPredictedMitotic(
    bool predictedMitotic)
{
  _predictedMitotic = predictedMitotic;
  _showMitotic = false;
}

void NucleusMarkerOrthoViewRendererCache::setContinuity(int continuity)
{
  _continuity = continuity;
}

void NucleusMarkerOrthoViewRendererCache::setPredictedContinuity(
    int predictedContinuity)
{
  _predictedContinuity = predictedContinuity;
}


NucleusMarkerOrthoViewRenderer::NucleusMarkerOrthoViewRenderer(
    NucleusMarker* marker, OrthoViewWidget* view)
        : SHSurfaceMarkerOrthoViewRenderer(marker, view)
{}

NucleusMarkerOrthoViewRenderer::NucleusMarkerOrthoViewRenderer(
    NucleusMarkerOrthoViewRenderer const &renderer)
        : SHSurfaceMarkerOrthoViewRenderer(renderer)
{}

NucleusMarkerOrthoViewRenderer::~NucleusMarkerOrthoViewRenderer()
{}
  
MarkerRenderer *NucleusMarkerOrthoViewRenderer::clone() const
{
  return new NucleusMarkerOrthoViewRenderer(*this);
}

void NucleusMarkerOrthoViewRendererCache::render(QPainter *painter) const
{
  if (painter == NULL || !painter->isActive()) return;

  if ((_showMitotic && _mitotic) || (!_showMitotic && _predictedMitotic))
      painter->drawEllipse(_point, _radius + 3.0f, _radius + 3.0f);

  painter->save();
  painter->setFont(QFont("Helvetica", _fontSize));
  if (_continuity >= 0)
      painter->drawText(
          QPointF(_point.x() + 3.0f, _point.y() - 3.0f),
          QString::number(_continuity));
  if (_predictedContinuity >= 0)
      painter->drawText(
          QPointF(_point.x() + 3.0f, _point.y() + (_fontSize + 3.0f)),
          QString::number(_predictedContinuity));
  painter->restore();

  SHSurfaceMarkerOrthoViewRendererCache::render(painter);
}

void NucleusMarkerOrthoViewRenderer::render(
    QXmlStreamWriter &svgStream,
    blitz::TinyVector<double,3> const &shapePx,
    blitz::TinyVector<double,3> const &lowerBoundUm,
    blitz::TinyVector<double,3> const &upperBoundUm,
    std::string const &font, double /*fontSizePt*/,
    iRoCS::ProgressReporter *pr) const
{
  if (p_marker->channel() == NULL)
  {
    std::cerr << "NucleusMarkerOrthoViewRenderer::render(): ERROR: Marker at "
              << "position " << p_marker->positionUm() << " has no asociated "
              << "channel!" << std::endl;
    return;
  }

  if (pr != NULL && !pr->updateProgress(pr->taskProgressMin())) return;

  OrthoViewWidget* view = static_cast<OrthoViewWidget*>(p_view);
  NucleusMarker* marker = static_cast<NucleusMarker*>(p_marker);

  double um2PxFactor = shapePx(2) / (upperBoundUm(2) - lowerBoundUm(2));

  // Get dimensions of current plane
  blitz::TinyVector<int,2> const &dimensions(
      view->orthoViewPlane(_orthogonalDimension)->dimensions());

  std::string viewId = view->orthoViewPlane(_orthogonalDimension)->viewId();

  // Get marker position in px
  blitz::TinyVector<float,3> markerPosPx(p_marker->positionUm() * um2PxFactor);
  blitz::TinyVector<float,3> posPx(
      (p_marker->positionUm() - lowerBoundUm) * um2PxFactor);

  // Get sphere radius in px
  double sphereRadiusPx = marker->radiusUm() * um2PxFactor;

  // Get slice position in px
  double slicePx = view->positionUm()(_orthogonalDimension) * um2PxFactor;

  // Get marker extent in slice direction
  double lbPx = markerPosPx(_orthogonalDimension) - sphereRadiusPx - 1.0;
  double ubPx = markerPosPx(_orthogonalDimension) + sphereRadiusPx + 1.0;
  if (marker->bw() != 0)
  {
    lbPx = std::min(
        lbPx, marker->boundingBoxLowerBoundUm()(_orthogonalDimension) *
        um2PxFactor);
    ubPx = std::max(
        ubPx, marker->boundingBoxUpperBoundUm()(_orthogonalDimension) *
        um2PxFactor);
  }
  
  // Check whether marker is visible, if not: skip rendering
  if (slicePx < lbPx || slicePx > ubPx) return;

  // Set marker color according to view mode
  bool draw;
  std::string markerColor = _getColorString(draw);
  if (!draw) return;

  svgStream.writeStartElement("g");
  svgStream.writeAttribute("id", (viewId + ": " + marker->id()).c_str());

  if (marker->bw() == 0 || (marker->bw() != 0 && view->showHelperLines()))
  {
    // If directly on marker draw cross
    if (std::abs(markerPosPx(_orthogonalDimension) - slicePx) <= 1.0f)
    {
      svgStream.writeEmptyElement("path");
      svgStream.writeAttribute(
          "d",
          "M" + QString::number(posPx(dimensions(1)) - sphereRadiusPx - 1.0) +
          " " + QString::number(posPx(dimensions(0))) +
          " L" + QString::number(posPx(dimensions(1)) + sphereRadiusPx + 1.0) +
          " " + QString::number(posPx(dimensions(0))));
      svgStream.writeAttribute(
          "style", ("stroke:" + markerColor + ";stroke-width:0.5pt").c_str());
      svgStream.writeEmptyElement("path");
      svgStream.writeAttribute(
          "d",
          "M" + QString::number(posPx(dimensions(1))) +
          " " + QString::number(posPx(dimensions(0)) - sphereRadiusPx - 1.0) +
          "L" + QString::number(posPx(dimensions(1))) +
          " " + QString::number(posPx(dimensions(0)) + sphereRadiusPx + 1.0));
      svgStream.writeAttribute(
          "style", ("stroke:" + markerColor + ";stroke-width:0.5pt").c_str());
    }
    else
    {
      svgStream.writeEmptyElement("path");
      svgStream.writeAttribute(
          "d",
          "M" + QString::number(posPx(dimensions(1)) - 0.5) +
          " " + QString::number(posPx(dimensions(0))) + " h1");
      svgStream.writeAttribute(
          "style", ("stroke:" + markerColor + ";stroke-width:0.5pt").c_str());
      svgStream.writeEmptyElement("path");
      svgStream.writeAttribute(
          "d",
          "M" + QString::number(posPx(dimensions(1))) +
          " " + QString::number(posPx(dimensions(0)) - 0.5) + " v1");
      svgStream.writeAttribute(
          "style", ("stroke:" + markerColor + ";stroke-width:0.5pt").c_str());
    }
  }
  
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
      
  /*-----------------------------------------------------------------------
   *  If coefficients are given draw solid segmentation and dashed circle
   *  otherwise draw solid circle
   *-----------------------------------------------------------------------*/
  if (marker->bw() != 0)
  {
    // Is there an intersection between marker bounding box and current slice?
    if (marker->boundingBoxLowerBoundUm()(_orthogonalDimension) * um2PxFactor <=
        slicePx &&
        marker->boundingBoxUpperBoundUm()(_orthogonalDimension) * um2PxFactor >=
        slicePx)
    {
      // Draw Polygon data
      svgStream.writeStartElement("g");
      svgStream.writeAttribute(
          "id", (viewId + "-" + marker->id() + ": segmentation").c_str());
      
      // For each triangle check for intersection with current slice
      QPointF p[2];
      for (size_t t = 0; t < marker->indices().size(); t += 3)
      {
        blitz::TinyVector<blitz::TinyVector<double,3>,3> tri(
            (*marker).vertices()[(*marker).indices()[3 * t]] +
            marker->positionUm(),
            (*marker).vertices()[(*marker).indices()[3 * t + 1]] +
            marker->positionUm(),
            (*marker).vertices()[(*marker).indices()[3 * t + 2]] +
            marker->positionUm());
        
        int nPoints = 0;
        for (int edge = 0; edge < 3; ++edge)
        {
          blitz::TinyVector<double,2> pos(
              intersectLineNPlane(
                  tri(edge), tri((edge + 1) % 3), _orthogonalDimension,
                  view->positionUm()(_orthogonalDimension)));
          
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
        if (nPoints == 2)
        {
          svgStream.writeEmptyElement("path");
          svgStream.writeAttribute(
              "d", "M" + QString::number(p[0].x()) + " " +
              QString::number(p[0].y()) + " L" + QString::number(p[1].x()) +
              " " + QString::number(p[1].y()));
          svgStream.writeAttribute(
              "style",
              ("stroke:" + markerColor + ";stroke-width:0.5pt").c_str());
        }
      }
    }
    svgStream.writeEndElement(); // g id="segmentation"
  }
  
  if (marker->bw() == 0 || (marker->bw() != 0 && view->showHelperLines()))
  {
    if (std::abs(markerPosPx(_orthogonalDimension) - slicePx) < sphereRadiusPx)
    {
      double circleRadiusPx =
          std::sqrt(sphereRadiusPx * sphereRadiusPx -
                    (markerPosPx(_orthogonalDimension) - slicePx) *
                    (markerPosPx(_orthogonalDimension) - slicePx));
      svgStream.writeEmptyElement("circle");
      svgStream.writeAttribute("cx", QString::number(posPx(dimensions(1))));
      svgStream.writeAttribute("cy", QString::number(posPx(dimensions(0))));
      svgStream.writeAttribute("r", QString::number(circleRadiusPx));
      if (marker->bw() == 0)
          svgStream.writeAttribute(
              "style", ("fill:none;stroke:" + markerColor +
                        ";stroke-width:0.5pt").c_str());
      else svgStream.writeAttribute(
          "style", ("fill:none;stroke:" + markerColor +
                    ";stroke-width:0.5pt;stroke-dasharray:5px, 2px").c_str());
      if (marker->mitotic())
      {
        svgStream.writeEmptyElement("circle");
        svgStream.writeAttribute("cx", QString::number(posPx(dimensions(1))));
        svgStream.writeAttribute("cy", QString::number(posPx(dimensions(0))));
        svgStream.writeAttribute("r", QString::number(circleRadiusPx + 1.0));
        if (marker->bw() == 0)
            svgStream.writeAttribute(
                "style",
                ("fill:none;stroke:" + markerColor +
                 ";stroke-width:0.5pt").c_str());
        else svgStream.writeAttribute(
            "style", ("fill:none;stroke:" + markerColor +
                      ";stroke-width:0.5pt;stroke-dasharray:5px, 2px").c_str());
      }
    }
  }

  svgStream.writeEndElement(); // g id=<markername>

  if (pr != NULL) pr->updateProgress(pr->taskProgressMax());
}

void NucleusMarkerOrthoViewRenderer::userInteractionEvent(
    UserInteractionEvent *event)
{
  OrthoViewUserInteractionEvent* e =
      reinterpret_cast<OrthoViewUserInteractionEvent*>(event);
  e->ignore();
 
  if (e->mouseEvent() != NULL &&
      e->mouseEvent()->type() == QEvent::MouseButtonRelease &&
      p_marker->channel() != NULL &&
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
      reinterpret_cast<NucleusMarker*>(p_marker)->setMitotic(false);
      reinterpret_cast<NucleusMarker*>(p_marker)->setContinuity(-1);
      e->accept();
      return;
    }
  }
  
  if (e->wheelEvent() != NULL)
  {
    if (e->wheelEvent()->modifiers().testFlag(Qt::ControlModifier))
    {
      SphereMarker *marker = reinterpret_cast<SphereMarker*>(p_marker);
      if (e->wheelEvent()->delta() < 0 && marker->radiusUm() >
          marker->channel()->model()->elementSizeUm()(2))
          marker->setRadiusUm(marker->radiusUm() -
                              marker->channel()->model()->elementSizeUm()(2));
      else marker->setRadiusUm(
          marker->radiusUm() + marker->channel()->model()->elementSizeUm()(2));
      e->accept();
      return;
    }

    if (e->wheelEvent()->modifiers().testFlag(Qt::ShiftModifier))
    {
      blitz::TinyVector<double,3> posUm(p_marker->positionUm());
      posUm(e->orthoViewPlane()->orthogonalDimension()) +=
          ((e->wheelEvent()->delta() < 0) ? -1.0 : 1.0) *
          p_marker->channel()->model()->elementSizeUm()(
              e->orthoViewPlane()->orthogonalDimension());
      p_marker->setPositionUm(posUm);
      e->accept();
      return;
    }
  }
}

void NucleusMarkerOrthoViewRenderer::updateCache(int direction) const
{
  if (!cacheUpdatesEnabled()) return;
  
  OrthoViewWidget* view = static_cast<OrthoViewWidget*>(p_view);
  NucleusMarker* marker = static_cast<NucleusMarker*>(p_marker);
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
      _cache(direction) = new NucleusMarkerOrthoViewRendererCache();
  NucleusMarkerOrthoViewRendererCache *cache =
      static_cast<NucleusMarkerOrthoViewRendererCache*>(_cache(direction));

  bool visible;
  _getColorString(visible);
  if (!visible)
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

  // Get view position in micrometers
  blitz::TinyVector<double,3> viewPosUm(p_view->positionUm());
  double viewPosUmInDirection = viewPosUm(direction);

  // Get the cross slice position in Pixel coordinates
  double viewSlicePosPx = view->um2Px(viewPosUmInDirection, direction);

  // Get the sphere radius in pixel scale
  double radiusPx = view->scaleToPx(marker->radiusUm());

  // Get the marker bounds
  double lbUmInDirection = marker->boundingBoxLowerBoundUm()(direction);
  double ubUmInDirection = marker->boundingBoxUpperBoundUm()(direction);
  
  // Cross Offset in Pixels
  double sliceOffsetPx = std::abs(viewSlicePosPx - markerPosPx(direction));

  // Marker is not visible ==> nothing to draw
  if ((marker->bw() == 0 ||
       lbUmInDirection > viewPosUmInDirection ||
       ubUmInDirection < viewPosUmInDirection) &&
      sliceOffsetPx > radiusPx + 3.0f)
  {
    cache->setActive(false);
    return;
  }
  
  // Get the corresponding screen (in-Plane) coordinates
  QPointF posPx = QPointF(markerPosPx(dims(1)), markerPosPx(dims(0)));
  
  cache->lines().clear();
  if (marker->channel()->selectedViewType() != 1)
      cache->setMitotic(marker->mitotic());
  else cache->setPredictedMitotic(marker->predictedMitotic());
  cache->setContinuity(marker->continuity());
  cache->setPredictedContinuity(marker->predictedContinuity());
  cache->setFontSize(view->fontSize());

  /*-----------------------------------------------------------------------
   *  Draw segmentation
   *-----------------------------------------------------------------------*/
  if (marker->bw() != 0)
  {
    // Is there an intersection between marker bounding box and current slice?
    if (lbUmInDirection <= viewPosUmInDirection &&
        ubUmInDirection >= viewPosUmInDirection)
    {
      std::vector<atb::SurfaceGeometry::VertexT> const &vertices =
          marker->vertices();
      std::vector<atb::SurfaceGeometry::IndexT> const &indices =
          marker->indices();

      // For each triangle check for intersection with current slice
      QPointF p[2];
      for (size_t t = 0; t < marker->indices().size(); t += 3)
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
          
          if (pos(0) == 0.0 && pos(1) ==
              std::numeric_limits<double>::infinity())
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
  }
  
  // The point must be set before the cross, otherwise the cross is disabled
  // It is not shown, but its position is used to determine the sphere
  // center
  cache->setPoint(posPx);

  if (sliceOffsetPx < radiusPx)
      cache->setRadius(
          std::sqrt(radiusPx * radiusPx - sliceOffsetPx * sliceOffsetPx));
  else cache->setRadius(0.0f);
  
  if (sliceOffsetPx <= 1.0f)
      cache->setCross(
          blitz::TinyVector<QLineF,2>(
              QLineF(posPx.x() - radiusPx - 3.0f, posPx.y(),
                     posPx.x() + radiusPx + 3.0f, posPx.y()),
              QLineF(posPx.x(), posPx.y() - radiusPx - 3.0f,
                     posPx.x(), posPx.y() + radiusPx + 3.0f)));

  cache->setActive(true);
  p_view->update();
}
