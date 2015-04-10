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

#include "IRoCSChannelSpecsOrthoViewRenderer.hh"

#include <QtGui/QApplication>
#include <QtCore/QThread>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtXml/QXmlStreamWriter>

#include "OrthoViewUserInteractionEvent.hh"

#include "MultiChannelModel.hh"
#include "IRoCSChannelSpecs.hh"
#include "OrthoViewWidget.hh"
#include "OrthoViewPlane.hh"
#include "SHSurfaceMarkerOrthoViewRenderer.hh"

IRoCSChannelSpecsOrthoViewRenderer::IRoCSChannelSpecsOrthoViewRenderer(
    IRoCSChannelSpecs* channel, OrthoViewWidget* view)
        : ChannelSpecsOrthoViewRenderer(channel, view), _startDragging(false),
          _isDragging(false), _draggedControlPoint(-1)
{}
  
IRoCSChannelSpecsOrthoViewRenderer::~IRoCSChannelSpecsOrthoViewRenderer()
{}

void IRoCSChannelSpecsOrthoViewRenderer::userInteractionEvent(
    const UserInteractionEvent&)
{}

void IRoCSChannelSpecsOrthoViewRenderer::render(QPainter* painter) const
{
  if (painter == NULL)
  {
    std::cerr << "IRoCSChannelSpecsOrthoViewRenderer::render(): "
              << "ERROR: No QPainter set!" << std::endl;
    return;
  }
  if (!painter->isActive()) return;
  
  OrthoViewWidget* view = static_cast<OrthoViewWidget*>(p_view);
  IRoCSChannelSpecs* channel = static_cast<IRoCSChannelSpecs*>(p_channel);
  MultiChannelModel* model = channel->model();
  
  // Get dimensions of current plane
  blitz::TinyVector<int,2> const &dims(
      view->orthoViewPlane(_orthogonalDimension)->dimensions());
  
  painter->save();
  painter->setRenderHint(QPainter::Antialiasing);
  painter->setPen(channel->qColor());
  
  double start = channel->p_rct->axisSpline().knot(0);
  double end = channel->p_rct->axisSpline().knot(
      channel->p_rct->axisSpline().nKnots() - 1);
  double step = (end - start) / 100.0;

  /*-----------------------------------------------------------------------
   *  Draw axis spline
   *-----------------------------------------------------------------------*/  
  QPainterPath mainAxisPath;

  blitz::TinyVector<double,3> posPx(
      view->um2Px(channel->p_rct->getAxisPosition(start)));
  mainAxisPath.moveTo(QPointF(posPx(dims(1)), posPx(dims(0))));
  for (double u = start + step; u <= end; u += step)
  {
    posPx = view->um2Px(channel->p_rct->getAxisPosition(u));
    mainAxisPath.lineTo(QPointF(posPx(dims(1)), posPx(dims(0))));
  }
  painter->drawPath(mainAxisPath);

  /*-----------------------------------------------------------------------
   *  If requested show also knots and control points
   *-----------------------------------------------------------------------*/
  if (view->showHelperLines())
  {
    atb::BSpline< blitz::TinyVector<double,3> > const &axisSpline =
        channel->p_rct->axisSpline();
    // Now add knots, control points and connecting lines
    for (size_t i = 0; i < axisSpline.nKnots(); ++i)
    {
      posPx = view->um2Px(axisSpline(axisSpline.knot(i)));
      painter->drawEllipse(QPointF(posPx(dims(1)), posPx(dims(0))), 3.0f, 3.0f);
      painter->drawText(
          QPointF(posPx(dims(1)), posPx(dims(0))),
          QString::number(axisSpline.knot(i)));
    }
    
    for (size_t i = 0; i < axisSpline.nControlPoints(); ++i)
    {
      posPx = view->um2Px(axisSpline.controlPoint(i));
      painter->drawRect(
          QRectF(QPointF(posPx(dims(1)), posPx(dims(0))) - QPointF(4.0f, 4.0f),
                 QSizeF(8.0f, 8.0f)));
    }
  }
  
  /*-----------------------------------------------------------------------
   *  Draw outer boundaries
   *-----------------------------------------------------------------------*/  
  // For each triangle check for intersection with current slice
  std::list< std::list< blitz::TinyVector<double,2> > > paths;
  
  for (size_t triangle = 0; triangle < channel->p_rct->indices().size();
       ++triangle)
  {
    blitz::TinyVector<blitz::TinyVector<double,3>,3> tri(
        (*channel->p_rct).vertices()(
            (*channel->p_rct).indices()(triangle)(0)),
        (*channel->p_rct).vertices()(
            (*channel->p_rct).indices()(triangle)(1)),
        (*channel->p_rct).vertices()(
            (*channel->p_rct).indices()(triangle)(2)));
    
    blitz::TinyVector<blitz::TinyVector<double,2>,2> pUm;
    int nPoints = 0;
    for (int edge = 0; edge < 3; ++edge)
    {
      blitz::TinyVector<double,2> pos(
          SHSurfaceMarkerOrthoViewRenderer::intersectLineNPlane(
              tri(edge), tri((edge + 1) % 3), _orthogonalDimension,
              view->positionUm()(_orthogonalDimension)));
      
      if (blitz::all(pos == std::numeric_limits<double>::infinity()))
          continue;
      
      if (pos(0) == 0.0 &&
          pos(1) == std::numeric_limits<double>::infinity())
      {
        pUm(0) = tri(edge)(dims(1)), tri(edge)(dims(0));
        pUm(1) = tri((edge + 1) % 3)(dims(1)),
            tri((edge + 1) % 3)(dims(0));
        nPoints = 2;
        break;
      }
      
      pUm(nPoints) = pos(1), pos(0);
      if (nPoints > 0 && blitz::all(pUm(nPoints) == pUm(0))) continue;
      nPoints++;
    }

    if (nPoints == 2)
    {
      // Check whether the segment extends an existing path
      bool segmentConsumed = false;
      for (std::list< std::list< blitz::TinyVector<double,2> > >::iterator
               it = paths.begin(); !segmentConsumed && it != paths.end(); ++it)
      {
        segmentConsumed = true;
        if (blitz::dot(pUm(0) - *it->begin(), pUm(0) - *it->begin()) < 1e-20)
            it->push_front(pUm(1));
        else if (blitz::dot(pUm(1) - *it->begin(), pUm(1) - *it->begin()) <
                 1e-20) it->push_front(pUm(0));
        else if (blitz::dot(pUm(0) - *(--it->end()), pUm(0) - *(--it->end())) <
                 1e-20) it->push_back(pUm(1));
        else if (blitz::dot(pUm(1) - *(--it->end()), pUm(1) - *(--it->end())) <
                 1e-20) it->push_back(pUm(0));
        else segmentConsumed = false;
      }

      // otherwise create new path
      if (!segmentConsumed)
      {
        std::list< blitz::TinyVector<double,2> > newPath;
        newPath.push_back(pUm(0));
        newPath.push_back(pUm(1));
        paths.push_back(newPath);
      }
    }
  }

  // Merge paths
  bool didMerge = false;
  do
  {
    didMerge = false;
    for (std::list< std::list< blitz::TinyVector<double,2> > >::iterator it =
             paths.begin(); it != paths.end(); ++it)
    {
      std::list< std::list< blitz::TinyVector<double,2> > >::iterator it2 = it;
      ++it2;
      for (; it2 != paths.end(); ++it2)
      {
        blitz::TinyVector<double,2> diff(*it->begin() - *it2->begin());
        if (blitz::dot(diff, diff) < 1e-20)
        {
          it2->pop_front();
          for (std::list< blitz::TinyVector<double,2> >::const_iterator itPath =
                   it2->begin(); itPath != it2->end(); ++itPath)
              it->push_front(*itPath);
          it2 = paths.erase(it2);
          --it2;
          didMerge = true;
        }
        else if (blitz::dot(*it->begin() - *(--it2->end()),
                            *it->begin() - *(--it2->end())) < 1e-20)
        {
          it2->pop_back();
          it->insert(it->begin(), it2->begin(), it2->end());
          it2 = paths.erase(it2);
          --it2;
          didMerge = true;
        }
        else if (blitz::dot(*(--it->end()) - *it2->begin(),
                            *(--it->end()) - *it2->begin()) < 1e-20)
        {
          it2->pop_front();
          it->insert(it->end(), it2->begin(), it2->end());
          it2 = paths.erase(it2);
          --it2;
          didMerge = true;
        }
        else if (blitz::dot(*(--it->end()) - *(--it2->end()),
                            *(--it->end()) - *(--it2->end())) < 1e-20)
        {
          it2->pop_back();
          for (std::list< blitz::TinyVector<double,2> >::const_iterator itPath =
                   it2->begin(); itPath != it2->end(); ++itPath)
              it->push_back(*itPath);
          it2 = paths.erase(it2);
          --it2;
          didMerge = true;
        }
      }
    }
  }
  while (didMerge);

  // Draw paths
  for (std::list< std::list< blitz::TinyVector<double,2> > >::const_iterator it
           = paths.begin(); it != paths.end(); ++it)
  {
    std::list< blitz::TinyVector<double,2> >::const_iterator it2 = it->begin();
    QPainterPath path;
    path.moveTo(
        QPointF(
            view->um2Px((*it2)(0), dims(1)), view->um2Px((*it2)(1), dims(0))));
    ++it2;
    for (; it2 != it->end(); ++it2)
    {
      path.lineTo(
          QPointF(
              view->um2Px((*it2)(0), dims(1)),
              view->um2Px((*it2)(1), dims(0))));
    }
    painter->drawPath(path);
  }

  /*-----------------------------------------------------------------------
   *  Extend axis spline by linear extrapolation
   *-----------------------------------------------------------------------*/  
  QPen pen(painter->pen());
  pen.setStyle(Qt::DashLine);
  painter->setPen(pen);

  // Compute left endpoint position and direction
  blitz::TinyVector<double,3> pos(channel->p_rct->axisSpline()(start));
  blitz::TinyVector<double,3> dir(
      -channel->p_rct->axisSpline().derivative(start));

  // Compute intersection of extended axis with volume boundaries
  // zDest = 0 || data.extentUm(0);
  // (x,y,z) = (x_0, y_0, z_0) + lambda * (d_x, d_y, d_z)
  // z_0 + lambda * d_z = zDest
  // lambda = (zDest - z_0) / d_z
  blitz::TinyVector<double,3> lambdaLower((model->lowerBoundUm() - pos) / dir);
  blitz::TinyVector<double,3> lambdaUpper((model->upperBoundUm() - pos) / dir);
  blitz::TinyVector<double,6> lambda(
      lambdaLower(0), lambdaLower(1), lambdaLower(2),
      lambdaUpper(0), lambdaUpper(1), lambdaUpper(2));

  for (int d = 0; d < 6; ++d)
  {
    if (lambda(d) > 0)
    {
      QPainterPath path;
      blitz::TinyVector<double,3> posUm(pos + (lambda(d) - 1e-10) * dir);
      if (blitz::all(posUm >= model->lowerBoundUm() &&
                     posUm <= model->upperBoundUm()))
      {
        posPx = view->um2Px(posUm);
        path.moveTo(QPointF(posPx(dims(1)), posPx(dims(0))));
        posPx = view->um2Px(channel->p_rct->axisSpline()(start));
        path.lineTo(QPointF(posPx(dims(1)), posPx(dims(0))));
        painter->drawPath(path);
        break;
      }
    }
  }

  // Do the same for the right end point
  pos = channel->p_rct->axisSpline()(end);
  dir = channel->p_rct->axisSpline().derivative(end);

  // Compute intersection of extended axis with volume boundaries
  lambdaLower = (model->lowerBoundUm() - pos) / dir;
  lambdaUpper = (model->upperBoundUm() - pos) / dir;
  lambda = lambdaLower(0), lambdaLower(1), lambdaLower(2),
      lambdaUpper(0), lambdaUpper(1), lambdaUpper(2);

  for (int d = 0; d < 6; ++d)
  {
    if (lambda(d) > 0)
    {
      QPainterPath path;
      blitz::TinyVector<double,3> posUm(pos + (lambda(d) - 1e-10) * dir);
      if (blitz::all(posUm >= model->lowerBoundUm() &&
                     posUm <= model->upperBoundUm()))
      {
        posPx = view->um2Px(channel->p_rct->axisSpline()(end));
        path.moveTo(QPointF(posPx(dims(1)), posPx(dims(0))));
        posPx = view->um2Px(posUm);
        path.lineTo(QPointF(posPx(dims(1)), posPx(dims(0))));
        painter->drawPath(path);
        break;
      }
    }
  }

  painter->restore();
}

void IRoCSChannelSpecsOrthoViewRenderer::render(
    QXmlStreamWriter &svgStream,
    blitz::TinyVector<double,3> const &shapePx,
    blitz::TinyVector<double,3> const &lowerBoundUm,
    blitz::TinyVector<double,3> const &upperBoundUm,
    std::string const &/*font*/, double /*fontSize*/,
    iRoCS::ProgressReporter *pr) const
{

  OrthoViewWidget* view = static_cast<OrthoViewWidget*>(p_view);
  IRoCSChannelSpecs* channel = static_cast<IRoCSChannelSpecs*>(p_channel);
  
  if (pr != NULL && !pr->updateProgress(pr->taskProgressMin())) return;

  // Get dimensions of current plane
  blitz::TinyVector<int,2> const &dimensions(
      view->orthoViewPlane(_orthogonalDimension)->dimensions());
  
  std::string viewId = static_cast<OrthoViewWidget*>(p_view)->orthoViewPlane(
      _orthogonalDimension)->viewId();

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
  
  double start = channel->p_rct->axisSpline().knot(0);
  double end = channel->p_rct->axisSpline().knot(
      channel->p_rct->axisSpline().nKnots() - 1);
  double step = (end - start) / 100.0;

  /*-----------------------------------------------------------------------
   *  Draw axis spline
   *-----------------------------------------------------------------------*/  
  blitz::TinyVector<float,3> posPx(
      (channel->p_rct->axisSpline()(start) - lowerBoundUm) * um2PxFactor);

  QString pathSpec = "M" + QString::number(posPx(dimensions(1))) + " " +
      QString::number(posPx(dimensions(0)));
  for (double u = start + step; u <= end; u += step)
  {
    posPx = (channel->p_rct->axisSpline()(u) - lowerBoundUm) * um2PxFactor;
    pathSpec = pathSpec + " L" + QString::number(posPx(dimensions(1))) + " " +
        QString::number(posPx(dimensions(0)));
  }

  svgStream.writeEmptyElement("path");
  svgStream.writeAttribute("d", pathSpec);
  svgStream.writeAttribute(
      "style", ("fill:none;stroke:" + markerColor +
                ";line-width:0.5pt").c_str());
  
  /*-----------------------------------------------------------------------
   *  Draw outer boundaries
   *-----------------------------------------------------------------------*/  
  // For each triangle check for intersection with current slice
  std::list< std::list< blitz::TinyVector<double,2> > > paths;
  
  for (size_t triangle = 0; triangle < channel->p_rct->indices().size();
       ++triangle)
  {
    blitz::TinyVector<blitz::TinyVector<double,3>,3> tri(
        (*channel->p_rct).vertices()(
            (*channel->p_rct).indices()(triangle)(0)),
        (*channel->p_rct).vertices()(
            (*channel->p_rct).indices()(triangle)(1)),
        (*channel->p_rct).vertices()(
            (*channel->p_rct).indices()(triangle)(2)));
    
    blitz::TinyVector<blitz::TinyVector<double,2>,2> pUm;
    int nPoints = 0;
    for (int edge = 0; edge < 3; ++edge)
    {
      blitz::TinyVector<double,2> pos(
          SHSurfaceMarkerOrthoViewRenderer::intersectLineNPlane(
              tri(edge), tri((edge + 1) % 3), _orthogonalDimension,
              view->positionUm()(_orthogonalDimension)));
      
      if (blitz::all(pos == std::numeric_limits<double>::infinity()))
          continue;
      
      if (pos(0) == 0.0 && pos(1) ==
          std::numeric_limits<double>::infinity())
      {
        pUm(0) = tri(edge)(dimensions(1)), tri(edge)(dimensions(0));
        pUm(1) = tri((edge + 1) % 3)(dimensions(1)),
            tri((edge + 1) % 3)(dimensions(0));
        nPoints = 2;
        break;
      }
      
      pUm(nPoints) = pos(1), pos(0);
      if (nPoints > 0 && blitz::all(pUm(nPoints) == pUm(0))) continue;
      nPoints++;
    }

    if (nPoints == 2)
    {
      // Check whether the segment extends an existing path
      bool segmentConsumed = false;
      for (std::list< std::list< blitz::TinyVector<double,2> > >::iterator
               it = paths.begin(); !segmentConsumed && it != paths.end(); ++it)
      {
        segmentConsumed = true;
        if (blitz::dot(pUm(0) - *it->begin(), pUm(0) - *it->begin()) < 1e-20)
            it->push_front(pUm(1));
        else if (blitz::dot(pUm(1) - *it->begin(), pUm(1) - *it->begin()) <
            1e-20) it->push_front(pUm(0));
        else if (blitz::dot(pUm(0) - *(--it->end()), pUm(0) - *(--it->end())) <
                 1e-20) it->push_back(pUm(1));
        else if (blitz::dot(pUm(1) - *(--it->end()), pUm(1) - *(--it->end())) <
                 1e-20) it->push_back(pUm(0));
        else segmentConsumed = false;
      }

      // otherwise create new path
      if (!segmentConsumed)
      {
        std::list< blitz::TinyVector<double,2> > newPath;
        newPath.push_back(pUm(0));
        newPath.push_back(pUm(1));
        paths.push_back(newPath);
      }
    }
  }
  
  // Merge paths
  bool didMerge = false;
  do
  {
    didMerge = false;
    for (std::list< std::list< blitz::TinyVector<double,2> > >::iterator it =
             paths.begin(); it != paths.end(); ++it)
    {
      std::list< std::list< blitz::TinyVector<double,2> > >::iterator it2 = it;
      ++it2;
      for (; it2 != paths.end(); ++it2)
      {
        if (blitz::dot(*it->begin() - *it2->begin(),
                       *it->begin() - *it2->begin()) < 1e-20)
        {
          it2->pop_front();
          for (std::list< blitz::TinyVector<double,2> >::const_iterator itPath =
                   it2->begin(); itPath != it2->end(); ++itPath)
              it->push_front(*itPath);
          it2 = paths.erase(it2);
          --it2;
          didMerge = true;
        }
        else if (blitz::dot(*it->begin() - *(--it2->end()),
                            *it->begin() - *(--it2->end())) < 1e-20)
        {
          it2->pop_back();
          it->insert(it->begin(), it2->begin(), it2->end());
          it2 = paths.erase(it2);
          --it2;
          didMerge = true;
        }
        else if (blitz::dot(*(--it->end()) - *it2->begin(),
                            *(--it->end()) - *it2->begin()) < 1e-20)
        {
          it2->pop_front();
          it->insert(it->end(), it2->begin(), it2->end());
          it2 = paths.erase(it2);
          --it2;
          didMerge = true;
        }
        else if (blitz::dot(*(--it->end()) - *(--it2->end()),
                            *(--it->end()) - *(--it2->end())) < 1e-20)
        {
          it2->pop_back();
          for (std::list< blitz::TinyVector<double,2> >::const_iterator itPath =
                   it2->begin(); itPath != it2->end(); ++itPath)
              it->push_back(*itPath);
          it2 = paths.erase(it2);
          --it2;
          didMerge = true;
        }
      }
    }
  }
  while (didMerge);

  // Draw paths
  for (std::list< std::list< blitz::TinyVector<double,2> > >::const_iterator it
           = paths.begin(); it != paths.end(); ++it)
  {
    std::list< blitz::TinyVector<double,2> >::const_iterator it2 = it->begin();
    QString path = "M " +
        QString::number(
            ((*it2)(0) - lowerBoundUm(dimensions(1))) * um2PxFactor) + " " +
        QString::number(
            ((*it2)(1) - lowerBoundUm(dimensions(0))) * um2PxFactor);
    ++it2;
    for (; it2 != it->end(); ++it2)
        path = path + " L " +
            QString::number(
                ((*it2)(0) - lowerBoundUm(dimensions(1))) * um2PxFactor)
            + " " +
            QString::number(
                ((*it2)(1) - lowerBoundUm(dimensions(0))) * um2PxFactor);
    svgStream.writeEmptyElement("path");
    svgStream.writeAttribute("d", path);
    svgStream.writeAttribute(
        "style", ("fill:none;stroke:" + markerColor +
                  ";line-width:0.5pt;").c_str());
  }

  /*-----------------------------------------------------------------------
   *  Extend axis spline by linear extrapolation
   *-----------------------------------------------------------------------*/  
  // Compute left endpoint position and direction
  blitz::TinyVector<double,3> pos(channel->p_rct->axisSpline()(start));
  blitz::TinyVector<double,3> dir(
      -channel->p_rct->axisSpline().derivative(start));

  // Compute intersection of extended axis with volume boundaries
  // zDest = lowerBoundUm(0) || upperBoundUm(0);
  // (x,y,z) = (x_0, y_0, z_0) + lambda * (d_x, d_y, d_z)
  // z_0 + lambda * d_z = zDest
  // lambda = (zDest - z_0) / d_z
  blitz::TinyVector<double,3> lambdaLower((lowerBoundUm - pos) / dir);
  blitz::TinyVector<double,3> lambdaUpper((upperBoundUm - pos) / dir);
  blitz::TinyVector<double,6> lambda(
      lambdaLower(0), lambdaLower(1), lambdaLower(2),
      lambdaUpper(0), lambdaUpper(1), lambdaUpper(2));

  for (int d = 0; d < 6; ++d)
  {
    if (lambda(d) > 0)
    {
      blitz::TinyVector<double,3> posUm(pos + (lambda(d) - 1e-10) * dir);
      if (blitz::all(posUm >= lowerBoundUm && posUm <= upperBoundUm))
      {
        posPx = (posUm - lowerBoundUm) * um2PxFactor;
        QString pathSpec =
            "M " + QString::number(posPx(dimensions(1))) + " " +
            QString::number(posPx(dimensions(0)));
        posPx = (channel->p_rct->axisSpline()(start) - lowerBoundUm) *
            um2PxFactor;
        pathSpec = pathSpec + " L" + QString::number(posPx(dimensions(1))) +
            " " + QString::number(posPx(dimensions(0)));
        svgStream.writeEmptyElement("path");
        svgStream.writeAttribute("d", pathSpec);
        svgStream.writeAttribute(
            "style",
            ("fill:none;stroke:" + markerColor +
             ";line-width:0.5pt;stroke-dasharray:3%, 1%").c_str());
        break;
      }
    }
  }

  // Do the same for the right end point
  pos = channel->p_rct->axisSpline()(end);
  dir = channel->p_rct->axisSpline().derivative(end);

  // Compute intersection of extended axis with volume boundaries
  lambdaLower = (lowerBoundUm - pos) / dir;
  lambdaUpper = (upperBoundUm - pos) / dir;
  lambda = lambdaLower(0), lambdaLower(1), lambdaLower(2),
      lambdaUpper(0), lambdaUpper(1), lambdaUpper(2);

  for (int d = 0; d < 6; ++d)
  {
    if (lambda(d) > 0)
    {
      blitz::TinyVector<double,3> posUm(pos + (lambda(d) - 1e-10) * dir);
      if (blitz::all(posUm >= lowerBoundUm && posUm <= upperBoundUm))
      {
        posPx = (channel->p_rct->axisSpline()(end) - lowerBoundUm) *
            um2PxFactor;
        QString pathSpec =
            "M " + QString::number(posPx(dimensions(1))) + " " +
            QString::number(posPx(dimensions(0)));
        posPx = (posUm - lowerBoundUm) * um2PxFactor;
        pathSpec = pathSpec + " L" + QString::number(posPx(dimensions(1))) +
            " " + QString::number(posPx(dimensions(0)));
        svgStream.writeEmptyElement("path");
        svgStream.writeAttribute("d", pathSpec);
        svgStream.writeAttribute(
            "style",
            ("fill:none;stroke:" + markerColor +
             ";line-width:0.5pt;stroke-dasharray:3%, 1%").c_str());
        break;
      }
    }
  }

  svgStream.writeEndElement(); // g id=<channelname>
  
  if (pr != NULL) pr->updateProgress(pr->taskProgressMax());
}
