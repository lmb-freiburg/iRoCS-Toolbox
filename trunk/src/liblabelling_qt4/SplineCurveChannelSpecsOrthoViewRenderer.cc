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

#include "SplineCurveChannelSpecsOrthoViewRenderer.hh"

#include <QtGui/QApplication>
#include <QtCore/QThread>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtXml/QXmlStreamWriter>

#include "OrthoViewUserInteractionEvent.hh"

#include "MultiChannelModel.hh"
#include "SplineCurveChannelSpecs.hh"
#include "OrthoViewWidget.hh"
#include "OrthoViewPlane.hh"

SplineCurveChannelSpecsOrthoViewRenderer::
SplineCurveChannelSpecsOrthoViewRenderer(
    SplineCurveChannelSpecs* channel, OrthoViewWidget* view)
        : ChannelSpecsOrthoViewRenderer(channel, view), _startDragging(false),
          _isDragging(false), _draggedControlPoint(-1)
{}
  
SplineCurveChannelSpecsOrthoViewRenderer::
~SplineCurveChannelSpecsOrthoViewRenderer()
{}

void SplineCurveChannelSpecsOrthoViewRenderer::userInteractionEvent(
    const UserInteractionEvent&)
{
  // const OrthoViewUserInteractionEvent* e =
  //     reinterpret_cast<const OrthoViewUserInteractionEvent*>(&event);
  // SplineCurveChannelSpecs* channel =
  //     reinterpret_cast<SplineCurveChannelSpecs*>(p_channel);
  // OrthoViewWidget* view =
  //     reinterpret_cast<OrthoViewWidget*>(p_view);

  // if (e->mouseEvent() != NULL)
  // {
  //   blitz::TinyVector<float,3> posUm(
  //       e->orthoViewPlane()->mousePositionUm(
  //           e->mouseEvent()->x(), e->mouseEvent()->y()));
  //   switch (e->mouseEvent()->type())
  //   {
  //   case QEvent::MouseButtonPress :
  //   {
  //     if (e->mouseEvent()->button() == Qt::LeftButton)
  //     {
  //       _draggedControlPoint =
  //           channel->closestControlPoint(
  //               posUm, 5.0 * channel->model()->elementSize()(2) /
  //               view->zoom(), true);
  //       if (_draggedMarker != NULL)
  //       {
  //         _startDragging = true;
  //       }
  //     }
  //     break;
  //   }
  //   case QEvent::MouseMove :
  //   {
  //     if (_startDragging && !_isDragging)
  //     {
  //       channel->selectControlPoint(_draggedControlPoint);
  //       _isDragging = true;
  //     }
  //     if (_isDragging)
  //     {
  //       _draggedControlPoint->setPositionUm(posUm);
  //     }
  //     break;
  //   }
  //   case QEvent::MouseButtonRelease :
  //   {
  //     _startDragging = false;
      
  //     if (_isDragging)
  //     {
  //       _isDragging = false;
  //       _draggedMarker = -1;
  //       break;
  //     }
  //   }
  // }
}

void SplineCurveChannelSpecsOrthoViewRenderer::render(QPainter* painter) const
{
  if (painter == NULL)
  {
    std::cerr << "SplineCurveChannelSpecsOrthoViewRenderer::render(): "
              << "ERROR: No QPainter set!" << std::endl;
    return;
  }
  if (!painter->isActive()) return;
  
  OrthoViewWidget* view = static_cast<OrthoViewWidget*>(p_view);
  SplineCurveChannelSpecs* channel = static_cast<SplineCurveChannelSpecs*>(
      p_channel);
  
  // Get dimensions of current plane
  blitz::TinyVector<int,2> const &dims(
      view->orthoViewPlane(_orthogonalDimension)->dimensions());
  
  painter->save();
  painter->setRenderHint(QPainter::Antialiasing);
  painter->setPen(channel->qColor());
  
  QPainterPath path;

  double start = channel->p_spline->knot(0);
  double end = channel->p_spline->knot(channel->p_spline->nKnots() - 1);
  double step = (end - start) / 100.0;

  blitz::TinyVector<float,3> posPx(view->um2Px((*channel->p_spline)(start)));
  QPointF posCanvas = QPointF(posPx(dims(1)), posPx(dims(0)));
  path.moveTo(posCanvas);
  for (double u = start + step; u <= end; u += step)
  {
    posPx = view->um2Px((*channel->p_spline)(u));
    posCanvas = QPointF(posPx(dims(1)), posPx(dims(0)));
    path.lineTo(posCanvas);
  }
  
  painter->drawPath(path);

  // Now add knots, control points and connecting lines
  for (size_t i = 0; i < channel->p_spline->nKnots(); ++i)
  {
    posPx = view->um2Px((*channel->p_spline)(channel->p_spline->knot(i)));
    painter->drawEllipse(
        QPointF(posPx(dims(1)), posPx(dims(0))), 3.0f, 3.0f);
    painter->drawText(
        QPointF(posPx(dims(1)), posPx(dims(0))),
        QString::number(channel->p_spline->knot(i)));
  }

  for (size_t i = 0; i < channel->p_spline->nControlPoints(); ++i)
  {
    posPx = view->um2Px(channel->p_spline->controlPoint(i));
    painter->drawRect(
        QRectF(QPointF(posPx(dims(1)), posPx(dims(0))) - QPointF(4.0f, 4.0f),
               QSizeF(8.0f, 8.0f)));
  }

  painter->restore();
}

void SplineCurveChannelSpecsOrthoViewRenderer::render(
    QXmlStreamWriter &svgStream,
    blitz::TinyVector<double,3> const &shapePx,
    blitz::TinyVector<double,3> const &lowerBoundUm,
    blitz::TinyVector<double,3> const &upperBoundUm,
    std::string const &font, double fontSizePt,
    iRoCS::ProgressReporter *pr) const
{
  OrthoViewWidget* view = static_cast<OrthoViewWidget*>(p_view);
  SplineCurveChannelSpecs* channel = static_cast<SplineCurveChannelSpecs*>(
      p_channel);

  if (pr != NULL && !pr->updateProgress(pr->taskProgressMin())) return;
  
  // Get dimensions of current plane
  blitz::TinyVector<int,2> const &dims(
      view->orthoViewPlane(_orthogonalDimension)->dimensions());
  
  std::string viewId = view->orthoViewPlane(_orthogonalDimension)->viewId();

  svgStream.writeStartElement("g");
  svgStream.writeAttribute("id", (viewId + ": " + channel->name()).c_str());
  double um2PxFactor = shapePx(2) / (upperBoundUm(2) - lowerBoundUm(2));
  
  std::string markerColor =
      ("#" +
       QString::number(channel->qColor().red(),
                       16).rightJustified(2, '0', true) +
       QString::number(channel->qColor().green(),
                       16).rightJustified(2, '0', true) +
       QString::number(channel->qColor().blue(),
                       16).rightJustified(2, '0', true)).toStdString();
  
  double start = channel->p_spline->knot(0);
  double end = channel->p_spline->knot(channel->p_spline->nKnots() - 1);
  double step = (end - start) / 100.0;

  blitz::TinyVector<float,3> posPx(
      ((*channel->p_spline)(start) - lowerBoundUm) * um2PxFactor);

  QString pathSpec = "M" + QString::number(posPx(dims(1))) + " " +
      QString::number(posPx(dims(0)));
  for (double u = start + step; u <= end; u += step)
  {
    posPx = ((*channel->p_spline)(u) - lowerBoundUm) * um2PxFactor;
    pathSpec = pathSpec + " L" + QString::number(posPx(dims(1))) + " " +
        QString::number(posPx(dims(0)));
  }

  svgStream.writeEmptyElement("path");
  svgStream.writeAttribute("d", pathSpec);
  svgStream.writeAttribute(
      "style", ("fill:none;stroke:" + markerColor +
                ";line-width:0.5pt").c_str());
  
  // Now add knots, control points and connecting lines
  for (size_t i = 0; i < channel->p_spline->nKnots(); ++i)
  {
    posPx = ((*channel->p_spline)(channel->p_spline->knot(i)) -
             lowerBoundUm) * um2PxFactor;
    svgStream.writeEmptyElement("circle");
    svgStream.writeAttribute("cx", QString::number(posPx(dims(1))));
    svgStream.writeAttribute("cy", QString::number(posPx(dims(0))));
    svgStream.writeAttribute("r", "3");
    svgStream.writeAttribute(
        "style", ("fill:none;stroke:" + markerColor +
                  ";line-width:0.5pt").c_str());
    svgStream.writeStartElement("text");
    svgStream.writeAttribute("x", QString::number(posPx(dims(1))));
    svgStream.writeAttribute("y", QString::number(posPx(dims(0))));
    svgStream.writeAttribute(
        "style", "font-size:" + QString::number(fontSizePt) +
        "pt;font-family:" + font.c_str() +
        ";stroke:none;fill:white;text-anchor:middle");
    svgStream.writeCharacters(QString::number(channel->p_spline->knot(i)));
    svgStream.writeEndElement(); // text
  }

  for (size_t i = 0; i < channel->p_spline->nControlPoints(); ++i)
  {
    posPx = (channel->p_spline->controlPoint(i) - lowerBoundUm) * um2PxFactor;
    svgStream.writeEmptyElement("rect");
    svgStream.writeAttribute("x", QString::number(posPx(dims(1)) - 4.0));
    svgStream.writeAttribute("y", QString::number(posPx(dims(0)) - 4.0));
    svgStream.writeAttribute("width", "8.0");
    svgStream.writeAttribute("height","8.0");
    svgStream.writeAttribute(
        "style", ("fill:none;stroke:" + markerColor +
                  ";line-width:0.5pt").c_str());
  }

  svgStream.writeEndElement(); // g id=<channelname>

  if (pr != NULL) pr->updateProgress(pr->taskProgressMax());
}
