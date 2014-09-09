/**************************************************************************
**       Title: SphereMarker-OrthoView Renderer
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    This class is the interface between the different View- and Marker
**    types. For each Marker type and each View type one such interface
**    class needs to be implemented. This is the specific implementation
**    for SphereMarker <=> OrthoView
**
**************************************************************************/

#include "SphereMarkerOrthoViewRenderer.hh"

#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>
#include <QtXml/QXmlStreamWriter>

#include "MultiChannelModel.hh"
#include "AnnotationChannelSpecs.hh"

#include "SphereMarker.hh"
#include "OrthoViewWidget.hh"
#include "OrthoViewPlane.hh"

#include "OrthoViewUserInteractionEvent.hh"


SphereMarkerOrthoViewRendererCache::SphereMarkerOrthoViewRendererCache()
        : PointMarkerOrthoViewRendererCache(), _radius(0.0f)
{}

SphereMarkerOrthoViewRendererCache::SphereMarkerOrthoViewRendererCache(
    SphereMarkerOrthoViewRendererCache const &cache)
        : PointMarkerOrthoViewRendererCache(cache), _radius(cache._radius)
{}

SphereMarkerOrthoViewRendererCache::~SphereMarkerOrthoViewRendererCache()
{}

MarkerOrthoViewRendererCache *SphereMarkerOrthoViewRendererCache::clone() const
{
  return new SphereMarkerOrthoViewRendererCache(*this);
}

void SphereMarkerOrthoViewRendererCache::render(QPainter *painter) const
{
  if (painter == NULL || !painter->isActive()) return;

  PointMarkerOrthoViewRendererCache::render(painter);

  if (_radius > 0.0f) painter->drawEllipse(_point, _radius, _radius);
}

void SphereMarkerOrthoViewRendererCache::setRadius(float radius)
{
  _radius = radius;
}


SphereMarkerOrthoViewRenderer::SphereMarkerOrthoViewRenderer(
    SphereMarker* marker, OrthoViewWidget* view)
        : PointMarkerOrthoViewRenderer(marker, view)
{}

SphereMarkerOrthoViewRenderer::SphereMarkerOrthoViewRenderer(
    SphereMarkerOrthoViewRenderer const &renderer)
        : PointMarkerOrthoViewRenderer(renderer)
{}

SphereMarkerOrthoViewRenderer::~SphereMarkerOrthoViewRenderer()
{}
  
MarkerRenderer *SphereMarkerOrthoViewRenderer::clone() const
{
  return new SphereMarkerOrthoViewRenderer(*this);
}

void SphereMarkerOrthoViewRenderer::render(
    QXmlStreamWriter &svgStream,
    blitz::TinyVector<double,3> const &shapePx,
    blitz::TinyVector<double,3> const &lowerBoundUm,
    blitz::TinyVector<double,3> const &upperBoundUm,
    std::string const &/*font*/, double /*fontSize*/,
    iRoCS::ProgressReporter *pr) const
{
  if (p_marker->channel() == NULL)
  {
    std::cerr << "SphereMarkerOrthoViewRenderer::render(): ERROR: Marker at "
              << "position " << p_marker->positionUm() << " has no asociated "
              << "channel!" << std::endl;
    return;
  }

  if (pr != NULL && !pr->updateProgress(pr->taskProgressMin())) return;

  OrthoViewWidget* view = static_cast<OrthoViewWidget*>(p_view);
  SphereMarker* marker = static_cast<SphereMarker*>(p_marker);

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
  
  // Check whether marker is visible, if not: skip rendering
  if (slicePx < lbPx || slicePx > ubPx) return;

  // Set marker color according to view mode
  bool draw;
  std::string markerColor = _getColorString(draw);
  if (!draw) return;

  svgStream.writeStartElement("g");
  svgStream.writeAttribute("id", (viewId + ": " + marker->id()).c_str());

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
    svgStream.writeAttribute(
        "style", ("fill:none;stroke:" + markerColor +
                  ";stroke-width:0.5pt").c_str());
  }

  svgStream.writeEndElement(); // g id=<markername>

  if (pr != NULL) pr->updateProgress(pr->taskProgressMax());
}

void SphereMarkerOrthoViewRenderer::userInteractionEvent(
    UserInteractionEvent *event)
{
  // std::cerr << "SphereMarkerOrthoViewRenderer (" << this
  //           << "): Caught UserInteractionEvent " << event << std::endl;

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
  
  if (e->wheelEvent() != NULL)
  {
    if (e->wheelEvent()->modifiers().testFlag(Qt::ControlModifier))
    {
      SphereMarker *marker = reinterpret_cast<SphereMarker*>(p_marker);
      if (e->wheelEvent()->delta() < 0 && marker->radiusUm() >
          marker->channel()->model()->elementSizeUm()(2))
          marker->setRadiusUm(
              marker->radiusUm() -
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
          p_marker->channel()->model()->elementSizeUm()(2);
      p_marker->setPositionUm(posUm);
      e->accept();
      return;
    }
  }
}

void SphereMarkerOrthoViewRenderer::updateCache(int direction) const
{
  if (!cacheUpdatesEnabled()) return;
  
  OrthoViewWidget* view = static_cast<OrthoViewWidget*>(p_view);
  SphereMarker* marker = static_cast<SphereMarker*>(p_marker);
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
      _cache(direction) = new SphereMarkerOrthoViewRendererCache();

  bool visible;
  _getColorString(visible);
  if (!visible)
  {
    _cache(direction)->setActive(false);
    return;
  }

  // Get dimensions of current plane
  blitz::TinyVector<int,2> const &dims(
      view->orthoViewPlane(direction)->dimensions());
  
  // Get marker position in Pixel coordinates
  blitz::TinyVector<double,3> markerPosPx(view->um2Px(marker->positionUm()));

  // Get the cross slice position in canvas coordinates
  double viewSlicePosPx = view->um2Px(view->positionUm()(direction), direction);

  // Get the sphere radius in Pixel coordinates
  double radiusPx = view->scaleToPx(marker->radiusUm());

  // Get the distance to the slice in pixels
  double sliceOffsetPx = std::abs(markerPosPx(direction) - viewSlicePosPx);

  // Marker not visible ==> nothing to paint
  if (sliceOffsetPx > radiusPx + 3.0f)
  {
    _cache(direction)->setActive(false);
    return;
  }

  // Get the corresponding screen (in-Plane) coordinates
  QPointF posPx = QPointF(markerPosPx(dims(1)), markerPosPx(dims(0)));

  SphereMarkerOrthoViewRendererCache *cache =
      static_cast<SphereMarkerOrthoViewRendererCache*>(_cache(direction));

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
