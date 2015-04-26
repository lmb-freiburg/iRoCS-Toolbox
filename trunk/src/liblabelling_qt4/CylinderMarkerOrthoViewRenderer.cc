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

#include "CylinderMarkerOrthoViewRenderer.hh"

#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>

#include "MultiChannelModel.hh"
#include "AnnotationChannelSpecs.hh"

#include "CylinderMarker.hh"
#include "OrthoViewWidget.hh"
#include "OrthoViewPlane.hh"

#include "OrthoViewUserInteractionEvent.hh"

CylinderMarkerOrthoViewRenderer::CylinderMarkerOrthoViewRenderer(
    CylinderMarker* marker, OrthoViewWidget* view)
        : MarkerOrthoViewRenderer(marker, view)
{}

CylinderMarkerOrthoViewRenderer::CylinderMarkerOrthoViewRenderer(
    CylinderMarkerOrthoViewRenderer const &renderer)
        : MarkerOrthoViewRenderer(renderer)
{}

CylinderMarkerOrthoViewRenderer::~CylinderMarkerOrthoViewRenderer()
{}
  
MarkerRenderer *CylinderMarkerOrthoViewRenderer::clone() const
{
  return new CylinderMarkerOrthoViewRenderer(*this);
}

void CylinderMarkerOrthoViewRenderer::render(QPainter* painter) const
{
  if (painter == NULL)
  {
    std::cerr << "CylinderMarkerOrthoViewRenderer::render(): ERROR: No "
              << "QPainter set!" << std::endl;
    return;
  }
  if (!painter->isActive()) return;

  OrthoViewWidget* view = static_cast<OrthoViewWidget*>(p_view);
  CylinderMarker* marker = static_cast<CylinderMarker*>(p_marker);
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

  // Get dimensions of current plane
  blitz::TinyVector<int,2> const &dimensions(
      view->orthoViewPlane(_orthogonalDimension)->dimensions());

  // Cylinder rotation
  blitz::Array<float,2> R(3, 3);
  R =
      1.0f, 0.0f, 0.0f, 
      0.0f, static_cast<float>(std::cos(marker->theta())),
      static_cast<float>(std::sin(marker->theta())),
      0.0f, static_cast<float>(-std::sin(marker->theta())),
      static_cast<float>(std::cos(marker->theta()));
  
  blitz::Array<float,2> R2(3, 3);
  R2 =
      static_cast<float>(std::cos(marker->phi())), 0.0f,
      static_cast<float>(std::sin(marker->phi())),
      0.0f, 1.0f, 0.0f,
      static_cast<float>(-std::sin(marker->phi())), 0.0f,
      static_cast<float>(std::cos(marker->phi()));
  
  R = atb::mmMult(R2, R);

  // Axis main direction
  blitz::TinyVector<float,3> e1(0.0, 0.0, 1.0);
  blitz::TinyVector<float,3> axis1(atb::mvMult(R, e1));
  
  // Cylinder axis start end end points
  blitz::TinyVector<float,3> axisStartUm(
      marker->positionUm() - marker->lengthUm() / 2.0 * axis1);
  blitz::TinyVector<float,3> axisEndUm(
      marker->positionUm() + marker->lengthUm() / 2.0 * axis1);

  // Bounding box corners
  blitz::TinyVector<float,3> axis1ProjectedUm(axis1);
  axis1ProjectedUm(_orthogonalDimension) = 0.0f;
  blitz::TinyVector<float,3> axis2ProjectedUm;
  axis2ProjectedUm(dimensions(0)) = -axis1ProjectedUm(dimensions(1));
  axis2ProjectedUm(dimensions(1)) = axis1ProjectedUm(dimensions(0));
  axis2ProjectedUm(_orthogonalDimension) = 0.0f;

  // Cylinder main axis projections
  painter->save();
  painter->setRenderHint(QPainter::Antialiasing);
  QTransform trafo = painter->transform();
  painter->resetTransform();

  blitz::TinyVector<float,3> cUm;
  QPointF corners[4];
  cUm = marker->positionUm() - marker->lengthUm() / 2.0 * axis1ProjectedUm -
      marker->radiusUm() * axis2ProjectedUm;
  corners[0] = trafo.map(
      QPointF(cUm(dimensions(1)) / model->elementSizeUm()(dimensions(1)),
              cUm(dimensions(0)) / model->elementSizeUm()(dimensions(0))));
  cUm = marker->positionUm() - marker->lengthUm() / 2.0 * axis1ProjectedUm +
      marker->radiusUm() * axis2ProjectedUm;
  corners[1] = trafo.map(
      QPointF(cUm(dimensions(1)) / model->elementSizeUm()(dimensions(1)),
              cUm(dimensions(0)) / model->elementSizeUm()(dimensions(0))));
  cUm = marker->positionUm() + marker->lengthUm() / 2.0 * axis1ProjectedUm +
      marker->radiusUm() * axis2ProjectedUm;
  corners[2] = trafo.map(
      QPointF(cUm(dimensions(1)) / model->elementSizeUm()(dimensions(1)),
              cUm(dimensions(0)) / model->elementSizeUm()(dimensions(0))));
  cUm = marker->positionUm() + marker->lengthUm() / 2.0 * axis1ProjectedUm -
      marker->radiusUm() * axis2ProjectedUm;
  corners[3] = trafo.map(
      QPointF(cUm(dimensions(1)) / model->elementSizeUm()(dimensions(1)),
              cUm(dimensions(0)) / model->elementSizeUm()(dimensions(0))));

  // Get marker position in Array coordinates
  blitz::TinyVector<float,3> posPx(
      marker->positionUm() / model->elementSizeUm());

  // Get start and end points in Array coordinates
  blitz::TinyVector<float,3> axisStartPx(axisStartUm / model->elementSizeUm());
  blitz::TinyVector<float,3> axisEndPx(axisEndUm / model->elementSizeUm());

  // Get the corresponding screen (in-Plane) coordinates
  QPointF posCanvas = trafo.map(
      QPointF(posPx(dimensions(1)), posPx(dimensions(0))));
  QPointF axisStartCanvas = trafo.map(
      QPointF(axisStartPx(dimensions(1)), axisStartPx(dimensions(0))));
  QPointF axisEndCanvas = trafo.map(
      QPointF(axisEndPx(dimensions(1)), axisEndPx(dimensions(0))));

  // And the slice offset in screen coordinates
  double markerSlicePosCanvas =
      (marker->positionUm()(_orthogonalDimension) /
       model->elementSizeUm()(_orthogonalDimension) + 0.5) *
      (model->elementSizeUm()(_orthogonalDimension) /
       model->elementSizeUm()(2)) * view->zoom();

  // And the cross offset also in screen coordinates
  double slicePosCanvas =
      (view->positionUm()(_orthogonalDimension) /
       model->elementSizeUm()(_orthogonalDimension) + 0.5) *
      (model->elementSizeUm()(_orthogonalDimension) /
       model->elementSizeUm()(2)) * view->zoom();

  if (!_setPenColor(painter, p_marker->selected()))
  {
    painter->restore();
    return;
  } 

  // Cross for the cylinder center position
  if (std::abs(markerSlicePosCanvas - slicePosCanvas) <= 1.0f)
  {
    painter->drawLine(
        QPointF(posCanvas.x() - 5.0f, posCanvas.y()),
        QPointF(posCanvas.x() + 5.0f, posCanvas.y()));
    painter->drawLine(
        QPointF(posCanvas.x(), posCanvas.y() - 5.0f),
        QPointF(posCanvas.x(), posCanvas.y() + 5.0f));
  }

  painter->save();
  QPen dashDotPen(painter->pen());
  dashDotPen.setStyle(Qt::DashDotLine);
  painter->setPen(dashDotPen);
  painter->drawLine(
      QPointF(axisStartCanvas.x(), axisStartCanvas.y()),
      QPointF(axisEndCanvas.x(), axisEndCanvas.y()));
  painter->restore();
  
  // Cylinder main axis bounding box
  painter->save();
  QPen dashPen(painter->pen());
  dashPen.setStyle(Qt::DashLine);
  painter->setPen(dashPen);
  painter->drawConvexPolygon(corners, 4);
  painter->restore();

  painter->restore();
}

void CylinderMarkerOrthoViewRenderer::render(
    QXmlStreamWriter &,
    blitz::TinyVector<double,3> const &,
    blitz::TinyVector<double,3> const &,
    blitz::TinyVector<double,3> const &,
    std::string const &, double, iRoCS::ProgressReporter *) const
{
  std::cerr << "Cylinder marker SVG rendering not yet implemented" << std::endl;
}

void CylinderMarkerOrthoViewRenderer::userInteractionEvent(
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
