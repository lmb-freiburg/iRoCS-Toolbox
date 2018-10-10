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

#include "OrthoViewPlane.hh"

#include <QtGui/QPainter>
#include <QtGui/QApplication>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>
#include <QtCore/QBuffer>
#include <QtXml/QXmlStreamWriter>

#include "MultiChannelModel.hh"
#include "OrthoViewOverlay.hh"
#include "OrthoViewWidget.hh"
#include "ChannelSpecsOrthoViewRenderer.hh"
#include "DataChannelSpecsOrthoViewRenderer.hh"
#include "VisualizationChannelSpecsOrthoViewRenderer.hh"
#include "RGBChannelSpecsOrthoViewRenderer.hh"
#include "OrthoViewUserInteractionEvent.hh"

// #include <blitz/tinyvec-et.h>

#include <algorithm>

OrthoViewPlane::OrthoViewPlane(
    OrthoViewWidget* orthoView, int orthogonalDimension, QWidget* parent)
        : QWidget(parent), p_orthoView(orthoView),
          _orthogonalDimension(orthogonalDimension),
          _dragVertical(false), _dragHorizontal(false),
          _isDragging(false)
{
  _data.resize(0, 0);
  setMouseTracking(true);
  setFixedSize(0, 0);
  switch (orthogonalDimension)
  {
  case 0:
    _dimensions(0) = 1;
    _dimensions(1) = 2;
    break;
  case 1:
    _dimensions(0) = 0;
    _dimensions(1) = 2;
    break;
  case 2:
    _dimensions(0) = 1;
    _dimensions(1) = 0;
    break;
  default:
    std::cerr << __FILE__ << ":" << __LINE__ << ": Invalid dimension "
              << orthogonalDimension << " given" << std::endl;
    exit(-1);
  }
}

OrthoViewPlane::~OrthoViewPlane()
{}

void OrthoViewPlane::addOverlay(OrthoViewOverlay* overlay)
{
  _overlays.push_back(overlay);
}

void OrthoViewPlane::removeOverlay(OrthoViewOverlay* overlay)
{
  std::list<OrthoViewOverlay*>::iterator it =
      std::find(_overlays.begin(), _overlays.end(), overlay);
  if(it != _overlays.end()) _overlays.erase(it);
}

OrthoViewWidget *OrthoViewPlane::orthoView()
{
  return p_orthoView;
}

blitz::TinyVector<double,3> OrthoViewPlane::mousePositionUm(int x, int y) const
{
  if (p_orthoView == NULL) return 0.0;
  blitz::TinyVector<double,3> mousePosUm(p_orthoView->positionUm());
  if (p_orthoView->model() == NULL ||
      p_orthoView->model()->elementSizeUm()(2) ==
      std::numeric_limits<double>::infinity()) return mousePosUm;
  mousePosUm(_dimensions(0)) = p_orthoView->px2Um(y, _dimensions(0));
  mousePosUm(_dimensions(1)) = p_orthoView->px2Um(x, _dimensions(1));
  return mousePosUm;
}

blitz::TinyVector<double,3> OrthoViewPlane::mousePositionPx(int x, int y) const
{
  if (p_orthoView == NULL) return 0.0;
  blitz::TinyVector<double,3> mousePosPx;
  mousePosPx(_orthogonalDimension) =
      p_orthoView->um2Px(p_orthoView->positionUm()(_orthogonalDimension),
                         _orthogonalDimension);
  mousePosPx(_dimensions(0)) = y;
  mousePosPx(_dimensions(1)) = x;
  return mousePosPx;
}

blitz::TinyVector<int,2> const &OrthoViewPlane::dimensions() const
{
  return _dimensions;
}

int OrthoViewPlane::orthogonalDimension() const
{
  return _orthogonalDimension;
}

std::string OrthoViewPlane::viewId() const
{
  if (_dimensions(0) == 0 && _dimensions(1) == 1) return "yz-view";
  if (_dimensions(0) == 0 && _dimensions(1) == 2) return "xz-view";
  if (_dimensions(0) == 1 && _dimensions(1) == 0) return "zy-view";
  if (_dimensions(0) == 1 && _dimensions(1) == 2) return "xy-view";
  if (_dimensions(0) == 2 && _dimensions(1) == 0) return "zx-view";
  if (_dimensions(0) == 2 && _dimensions(1) == 1) return "yx-view";
  std::cerr << __FILE__ << ":" << __LINE__ << " error: the set _dimensions "
            << _dimensions << " are no axes for a proper view" << std::endl;
  exit(-1);
  return "invalid view";
}

blitz::Array<blitz::TinyVector<unsigned char,4>,2> const &
OrthoViewPlane::image() const {
  return _data;
}

void OrthoViewPlane::adjustSize()
{
  MultiChannelModel *model = p_orthoView->model();
  if (model == NULL)
  {
#ifdef DEBUG
    std::cerr << "OrthoViewPlane::adjustSize() aborted. Model is NULL."
              << std::endl;
#endif
    return;
  }
  if (blitz::any(
          model->elementSizeUm() == std::numeric_limits<double>::infinity()))
  {
#ifdef DEBUG
    std::cerr << "OrthoViewPlane::adjustSize() aborted. Model element size "
              << "uninitialized." << std::endl;
#endif
    return;
  }
  int newWidth = static_cast<int>(
      p_orthoView->scaleToPx(
          model->boundingBoxShapeUm()(_dimensions(0)))) + 1;
  int newHeight = static_cast<int>(
      p_orthoView->scaleToPx(
          model->boundingBoxShapeUm()(_dimensions(1)))) + 1;

  if (width() == newWidth && height() == newHeight) return;

#ifdef DEBUG
  std::cerr << "OrthoViewPlane::adjustSize() current size: " << height()
            << " x " << width() << " px" << std::endl;
#endif

  blitz::TinyVector<atb::BlitzIndexT,3> modelShapePx(
      model->boundingBoxShapeUm() / model->elementSizeUm());
  if (_data.extent(0) != modelShapePx(_dimensions(0)) ||
      _data.extent(1) != modelShapePx(_dimensions(1)))
  {
    if (blitz::all(modelShapePx == 0)) _data.free();
    else
    {
      _data.resize(
          modelShapePx(_dimensions(0)), modelShapePx(_dimensions(1)));
      updateData();
    }
  }
  setFixedSize(newHeight, newWidth);

#ifdef DEBUG
  std::cerr << "... OrthoViewPlane::adjustSize(): New size: " << height()
            << " x " << width() << " px" << std::endl;
#endif

  bool enable = p_orthoView->updatesEnabled();
  p_orthoView->setUpdatesEnabled(false);
  for (std::vector<ChannelSpecs*>::iterator it = model->begin();
       it != model->end(); ++it)
  {
    ChannelSpecsOrthoViewRenderer *rend =
        static_cast<ChannelSpecsOrthoViewRenderer*>(
            (*it)->renderer(p_orthoView));
    if ((*it)->visible() && rend != NULL)
        rend->updateCache(_orthogonalDimension);
  }
  p_orthoView->setUpdatesEnabled(enable);
  update();
}

void OrthoViewPlane::updateData()
{
#ifdef DEBUG
  std::cerr << "OrthoViewPlane::updateData() for direction "
            << _orthogonalDimension << std::endl;
#endif
  MultiChannelModel *model = p_orthoView->model();
  if (model == NULL)
  {
#ifdef DEBUG
    std::cerr << __FILE__ << ":" << __LINE__ << ": The OrthoView has no "
              <<"associated model" << std::endl;
#endif
    return;
  }
  if (blitz::any(model->elementSizeUm() ==
                 std::numeric_limits<double>::infinity())) return;

  blitz::TinyVector<int,2> &dims = _dimensions;
  blitz::TinyVector<atb::BlitzIndexT,3> lbModelPx(
      blitz::floor(model->lowerBoundUm() / model->elementSizeUm()));
  blitz::TinyVector<atb::BlitzIndexT,3> ubModelPx(
      blitz::ceil(model->upperBoundUm() / model->elementSizeUm()));
  blitz::TinyVector<atb::BlitzIndexT,3> modelShapePx(ubModelPx - lbModelPx + 1);
  _data.resize(modelShapePx(dims(0)), modelShapePx(dims(1)));
  _data = blitz::TinyVector<unsigned char,4>(0, 0, 0, 255);
  for (std::vector<ChannelSpecs*>::iterator it = model->begin();
       it != model->end(); ++it)
  {
    if (!(*it)->visible() || (*it)->renderer(p_orthoView) == NULL) continue;

    blitz::Array<blitz::TinyVector<float,3>,2> const *data = NULL;
    blitz::TinyVector<atb::BlitzIndexT,2> lbDest;
    switch ((*it)->channelType())
    {
    case ChannelSpecs::Data :
    {
      DataChannelSpecsOrthoViewRenderer *renderer =
          static_cast<DataChannelSpecsOrthoViewRenderer *>(
              (*it)->renderer(p_orthoView));
      data = &renderer->cache(_orthogonalDimension);
      lbDest =
          renderer->cacheOffsetPx()(dims(0)),
          renderer->cacheOffsetPx()(dims(1));
      break;
    }
    case ChannelSpecs::Visualization :
    {
      VisualizationChannelSpecsOrthoViewRenderer *renderer =
          static_cast<VisualizationChannelSpecsOrthoViewRenderer *>(
              (*it)->renderer(p_orthoView));
      data = &renderer->cache(_orthogonalDimension);
      lbDest =
          renderer->cacheOffsetPx()(dims(0)),
          renderer->cacheOffsetPx()(dims(1));
      break;
    }
    case ChannelSpecs::RGB :
    {
      RGBChannelSpecsOrthoViewRenderer *renderer =
          static_cast<RGBChannelSpecsOrthoViewRenderer *>(
              (*it)->renderer(p_orthoView));
      data = &renderer->cache(_orthogonalDimension);
      lbDest =
          renderer->cacheOffsetPx()(dims(0)),
          renderer->cacheOffsetPx()(dims(1));
      break;
    }
    default:
      continue;
    }

    blitz::TinyVector<atb::BlitzIndexT,2> lbSrc(atb::BlitzIndexT(0));
    blitz::TinyVector<atb::BlitzIndexT,2> ubSrc(data->shape() - 1);
    lbDest -= blitz::TinyVector<atb::BlitzIndexT,2>(
        lbModelPx(dims(0)), lbModelPx(dims(1)));
    blitz::TinyVector<atb::BlitzIndexT,2> ubDest(lbDest + data->shape() - 1);

    for (int d = 0; d < 2; ++d)
    {
      if (lbDest(d) < 0)
      {
        lbSrc(d) = -lbDest(d);
        lbDest(d) = 0;
      }
      if (ubDest(d) >= _data.extent(d))
      {
        ubSrc(d) -= ubDest(d) - _data.extent(d) + 1;
        ubDest(d) = _data.extent(d) - 1;
      }
    }

    if (blitz::any(lbDest > ubDest || lbSrc > ubSrc)) continue;

    blitz::RectDomain<2> srcRange(lbSrc, ubSrc);
    blitz::RectDomain<2> destRange(lbDest, ubDest);

#ifdef DEBUG
    std::cerr << "Writing cache slice of channel " << (*it)->name()
              << " with shape " << data->shape() << " to OrthoViewPlane with "
              << "shape " << _data.shape() << std::endl;
    std::cerr << "  Used bounds are: " << std::endl;
    std::cerr << "  channel = " << lbSrc << "  -  " << ubSrc << std::endl;
    std::cerr << "  view    = " << lbDest << "  -  " << ubDest << std::endl;
#endif

    _data[2](destRange) = blitz::where(
        (*data)[0](srcRange) * 255.0f +
        blitz::cast<float>(_data[2](destRange)) <= 255.0f,
        blitz::cast<unsigned char>(
            (*data)[0](srcRange) * 255.0f +
            blitz::cast<float>(_data[2](destRange))), 255);
    _data[1](destRange) = blitz::where(
        (*data)[1](srcRange) * 255.0f +
        blitz::cast<float>(_data[1](destRange)) <= 255.0f,
        blitz::cast<unsigned char>(
            (*data)[1](srcRange) * 255.0f +
            blitz::cast<float>(_data[1](destRange))), 255);
    _data[0](destRange) = blitz::where(
        (*data)[2](srcRange) * 255.0f +
        blitz::cast<float>(_data[0](destRange)) <= 255.0f,
        blitz::cast<unsigned char>(
            (*data)[2](srcRange) * 255.0f +
            blitz::cast<float>(_data[0](destRange))), 255);
  }
}

void OrthoViewPlane::paint(QPainter &painter)
{
  if (!painter.isActive()) return;
  MultiChannelModel *model = p_orthoView->model();
  if (model == NULL) return;
  if (blitz::any(model->elementSizeUm() ==
                 std::numeric_limits<double>::infinity()))
  {
#ifdef DEBUG
    std::cerr << "OrthoViewPlane::paint() aborted. Element size not "
              << "initialized." << std::endl;
#endif
    return;
  }

  if (_data.size() == 0)
  {
    adjustSize();
    updateData();
  }

  // Data channels
  painter.scale(
      p_orthoView->zoom() *
      p_orthoView->model()->elementSizeUm()(_dimensions(1)) /
      p_orthoView->model()->elementSizeUm()(2),
      p_orthoView->zoom() *
      p_orthoView->model()->elementSizeUm()(_dimensions(0)) /
      p_orthoView->model()->elementSizeUm()(2));
  if (p_orthoView->interpolation())
      painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

  painter.drawImage(QPointF(0.0f, 0.0f),
                    QImage(reinterpret_cast<unsigned char*>(_data.data()),
                           static_cast<int>(_data.extent(1)),
                           static_cast<int>(_data.extent(0)),
                           QImage::Format_RGB32));
  painter.resetTransform();

  // Annotation and Spline Curve channels
  for (std::vector<ChannelSpecs*>::iterator it = model->begin();
       it != model->end(); ++it)
  {
    if ((*it)->visible() &&
        ((*it)->channelType() == ChannelSpecs::Annotation ||
         (*it)->channelType() == ChannelSpecs::SplineCurve ||
         (*it)->channelType() == ChannelSpecs::IRoCS ||
         (*it)->channelType() == ChannelSpecs::IRoCSSCT))
    {
      ChannelSpecsOrthoViewRenderer *renderer =
          static_cast<ChannelSpecsOrthoViewRenderer*>(
              (*it)->renderer(p_orthoView));
      if (renderer == NULL) continue;
      renderer->setOrthogonalDimension(_orthogonalDimension);
      renderer->render(&painter);
    }
  }

  // overlays
  for(std::list<OrthoViewOverlay*>::iterator it = _overlays.begin();
      it != _overlays.end(); ++it)
  {
    (*it)->setDimensions(_dimensions);
    (*it)->render(&painter);
  }

  // And the cross hair
  blitz::TinyVector<float,3> posPx(
      p_orthoView->um2Px(p_orthoView->positionUm()));
  painter.setPen(Qt::red);
  painter.drawLine(
      QPointF(0, posPx(_dimensions(0))),
      QPointF(width(), posPx(_dimensions(0))));
  painter.drawLine(
      QPointF(posPx(_dimensions(1)), 0),
      QPointF(posPx(_dimensions(1)), height()));
}

void OrthoViewPlane::paint(
    QXmlStreamWriter &svgStream,
    blitz::TinyVector<double,3> const &shapePx,
    blitz::TinyVector<double,3> const &lowerBoundUm,
    blitz::TinyVector<double,3> const &upperBoundUm,
    std::string const &font, double fontSizePt,
    iRoCS::ProgressReporter *pr)
{
  MultiChannelModel *model = p_orthoView->model();
  if (model == NULL || blitz::any(
          model->elementSizeUm() == std::numeric_limits<double>::infinity()))
  {
    if (pr != NULL) pr->abort();
    return;
  }

  if (_data.size() == 0)
  {
    adjustSize();
    updateData();
  }

  int pMin = (pr != NULL) ? pr->taskProgressMin() : 0;
  int pScale = (pr != NULL) ? pr->taskProgressMax() - pMin : 100;
  if (pr != NULL && !pr->updateProgress(pMin)) return;

  // Data channels

  // Compute conversion factor from micrometers to pixels in the output
  // SVG image
  double um2PxFactor = shapePx(2) / (upperBoundUm(2) - lowerBoundUm(2));

  // Compute pixel range in the intensity cache
  blitz::TinyVector<double,3> lbPx(
      (lowerBoundUm - model->lowerBoundUm()) / model->elementSizeUm());
  blitz::TinyVector<double,3> ubPx(
      (upperBoundUm - model->lowerBoundUm()) / model->elementSizeUm());
  blitz::TinyVector<atb::BlitzIndexT,3> lbPxInt(blitz::floor(lbPx + 0.5));
  blitz::TinyVector<atb::BlitzIndexT,3> ubPxInt(blitz::floor(ubPx + 0.5));

  // Get the source range for this OrthoViewPlane
  blitz::TinyVector<atb::BlitzIndexT,2> lbPxInt2(
      lbPxInt(_dimensions(0)), lbPxInt(_dimensions(1)));
  blitz::TinyVector<atb::BlitzIndexT,2> ubPxInt2(
      ubPxInt(_dimensions(0)), ubPxInt(_dimensions(1)));
  blitz::RectDomain<2> srcDomain(lbPxInt2, ubPxInt2 - 1);

  std::cerr << "Rendering intensity data from " << lbPxInt2 << " to "
            << ubPxInt2 << " which should be contained in 2 [0, 0] - "
            << _data.shape() << std::endl;

  blitz::TinyVector<double,3> elementSizePx(
      shapePx / ((upperBoundUm - lowerBoundUm) / model->elementSizeUm()));
  blitz::TinyVector<double,3> subPixelShift(
      lbPx - blitz::TinyVector<double,3>(lbPxInt) + 0.5 * elementSizePx);

  blitz::Array<blitz::TinyVector<unsigned char,4>,2> cropped(
      blitz::TinyVector<atb::BlitzIndexT,2>(ubPxInt2 - lbPxInt2));
  cropped = _data(srcDomain);

  // Create buffer for base 64 encoding
  QByteArray pngByteArray;
  QBuffer buffer(&pngByteArray);

  if (pr != NULL && !pr->updateProgress(
          static_cast<int>(pMin + 0.05 * pScale))) return;

  // Write cropped portion of the view to the buffer
  QImage image(reinterpret_cast<unsigned char*>(cropped.data()),
               static_cast<int>(cropped.extent(1)),
               static_cast<int>(cropped.extent(0)),
               QImage::Format_RGB32);
  image.save(&buffer, "PNG");

  if (pr != NULL && !pr->updateProgress(
          static_cast<int>(pMin + 0.1 * pScale))) return;

  // Write raw data base64 encoded into svgStream
  svgStream.writeEmptyElement("image");
  svgStream.writeAttribute("id", (viewId() + ": data").c_str());
  svgStream.writeAttribute("x", "0");
  svgStream.writeAttribute("y", "0");
  svgStream.writeAttribute(
      "height", QString::number(shapePx(_dimensions(0))) + "px");
  svgStream.writeAttribute(
      "width", QString::number(shapePx(_dimensions(1))) + "px");
  svgStream.writeAttribute("xlink:href", "data:image/png;base64," +
                           QString(pngByteArray.toBase64()));

  if (pr != NULL && !pr->updateProgress(
          static_cast<int>(pMin + 0.2 * pScale))) return;

  svgStream.writeStartElement("g");
  svgStream.writeAttribute("id", (viewId() + ": annotations").c_str());
  svgStream.writeAttribute(
      "transform", "translate(" +
      QString::number(subPixelShift(_dimensions(0))) + "," +
      QString::number(subPixelShift(_dimensions(1))) + ")");

  // Now the annotation channels
  int c = 0;
  for (std::vector<ChannelSpecs*>::iterator it = model->begin();
       it != model->end(); ++it, ++c)
  {
    if (pr != NULL)
    {
      if (pr->isAborted()) continue;
      pr->setTaskProgressMin(
          static_cast<int>(
              pMin + (0.2 + 0.75 * static_cast<double>(c) /
                      static_cast<double>(model->nChannels())) * pScale));
      pr->setTaskProgressMax(
          static_cast<int>(
              pMin + (0.2 + 0.75 * static_cast<double>(c + 1) /
                      static_cast<double>(model->nChannels())) * pScale));
    }
    if ((*it)->visible() &&
        ((*it)->channelType() == ChannelSpecs::Annotation ||
         (*it)->channelType() == ChannelSpecs::SplineCurve ||
         (*it)->channelType() == ChannelSpecs::IRoCS ||
         (*it)->channelType() == ChannelSpecs::IRoCSSCT))
    {
      ChannelSpecsOrthoViewRenderer* renderer =
          static_cast<ChannelSpecsOrthoViewRenderer*>(
              (*it)->renderer(p_orthoView));
      renderer->setOrthogonalDimension(_orthogonalDimension);
      renderer->render(svgStream, shapePx, lowerBoundUm, upperBoundUm, font,
                       fontSizePt, pr);
    }
  }

  svgStream.writeEndElement(); // g id="annotations"

  // And the cross hair
  svgStream.writeStartElement("g");
  svgStream.writeAttribute("id", (viewId() + ": cross hair").c_str());
  svgStream.writeEmptyElement("path");
  svgStream.writeAttribute(
      "d", "M0 " + QString::number(
          (p_orthoView->positionUm()(_dimensions(0)) -
           lowerBoundUm(_dimensions(0))) * um2PxFactor +
          subPixelShift(_dimensions(0))) + " h" +
      QString::number(shapePx(_dimensions(1))));
  svgStream.writeAttribute(
      "style", "stroke:red;stroke-width:0.5pt");
  svgStream.writeEmptyElement("path");
  svgStream.writeAttribute(
      "d", "M" + QString::number(
          (p_orthoView->positionUm()(_dimensions(1)) -
           lowerBoundUm(_dimensions(1))) * um2PxFactor +
          subPixelShift(_dimensions(1))) + " 0 v" +
      QString::number(shapePx(_dimensions(0))));
  svgStream.writeAttribute(
      "style", "stroke:red;stroke-width:0.5pt");
  svgStream.writeEndElement(); // g id="cross hair"
  if (pr != NULL) pr->updateProgress(pMin + pScale);
}

void OrthoViewPlane::paintEvent(QPaintEvent*)
{
#ifdef DEBUG
  std::cerr << "OrthoViewPlane::paintEvent(QPaintEvent*)" << std::endl;
#endif
#ifdef DEBUG
  if (p_orthoView == NULL)
  {
    std::cerr << __FILE__ << ":" << __LINE__
              << ": OrthoViewPlane::paintEvent() "
              << "called for an OrthoViewPlane that does not belong to an "
              << "OrthoViewWidget." << std::endl;
    return;
  }
  if (p_orthoView->model() == NULL)
  {
    std::cerr << __FILE__ << ":" << __LINE__
              << ": OrthoViewPlane::paintEvent() "
              << "called for an OrthoView that is not associated to a model."
              << std::endl;
    return;
  }
  if (blitz::any(p_orthoView->model()->elementSizeUm() ==
                 std::numeric_limits<double>::infinity()))
  {
    std::cerr << "OrthoViewPlane::paintEvent() aborted. Element size not "
              << "initialized." << std::endl;
    return;
  }
#else
  if (blitz::any(p_orthoView->model()->elementSizeUm() ==
                 std::numeric_limits<double>::infinity())) return;
#endif

  QPainter painter(this);
  paint(painter);
}

void OrthoViewPlane::mousePressEvent(QMouseEvent* e)
{
  // If the user left clicked on a line of the crosshair, indicate that
  // subsequent moves are drag events and accept the event
  if (e->button() == Qt::LeftButton)
  {
    if (p_orthoView != NULL)
    {
      if (std::abs(mousePositionPx(e->x(), e->y())(_dimensions(1)) -
                   p_orthoView->um2Px(
                       p_orthoView->positionUm()(_dimensions(1)),
                       _dimensions(1))) < 3) _dragHorizontal = true;
      if (std::abs(mousePositionPx(e->x(), e->y())(_dimensions(0)) -
                   p_orthoView->um2Px(
                       p_orthoView->positionUm()(_dimensions(0)),
                       _dimensions(0))) < 3) _dragVertical = true;
      if (_dragVertical || _dragHorizontal)
      {
        e->accept();
        return;
      }
    }
  }

  if (p_orthoView != NULL && p_orthoView->model() != NULL &&
      p_orthoView->model()->selectedChannel() != NULL)
  {
    ChannelSpecsRenderer *renderer =
        p_orthoView->model()->selectedChannel()->renderer(p_orthoView);
    if (renderer != NULL)
    {
      OrthoViewUserInteractionEvent event(this, e, NULL);
      qApp->sendEvent(renderer, &event);
    }
#ifdef DEBUG
    else
        std::cerr << "No associated renderer for channel "
                  << p_orthoView->model()->selectedChannel()->name()
                  << " and this orthoview found" << std::endl;
#endif
  }
}

void OrthoViewPlane::mouseMoveEvent(QMouseEvent* e)
{
  if (p_orthoView != NULL)
  {
    // Setup Cursor shape according to cross (independent of
    // selection/dragging mode)
    if (std::abs(mousePositionPx(e->x(), e->y())(_dimensions(1)) -
                 p_orthoView->um2Px(
                     p_orthoView->positionUm()(_dimensions(1)),
                     _dimensions(1))) < 3)
    {
      if (std::abs(mousePositionPx(e->x(), e->y())(_dimensions(0)) -
                   p_orthoView->um2Px(
                       p_orthoView->positionUm()(_dimensions(0)),
                       _dimensions(0))) < 3)
          setCursor(Qt::SizeAllCursor);
      else setCursor(Qt::SplitHCursor);
    }
    else
    {
      if (std::abs(mousePositionPx(e->x(), e->y())(_dimensions(0)) -
                   p_orthoView->um2Px(
                       p_orthoView->positionUm()(_dimensions(0)),
                       _dimensions(0))) < 3)
          setCursor(Qt::SplitVCursor);
      else setCursor(Qt::CrossCursor);
    }

    p_orthoView->model()->updateInfoWidget(mousePositionUm(e->x(), e->y()));

    // Check whether the user wants to drag a line or the cross
    // If so, set the isDragging flag to switch from selection in
    // dragging mode
    if (_dragHorizontal || _dragVertical)
    {
      if (e->buttons().testFlag(Qt::LeftButton)) // Actually dragging
      {
        blitz::TinyVector<double,3> newPos(p_orthoView->positionUm());
        if (_dragHorizontal && !_dragVertical)
        {
          newPos(_dimensions(1)) =
              mousePositionUm(e->x(), e->y())(_dimensions(1));

          MultiChannelModel *model = p_orthoView->model();
          if (model != NULL)
          {
            std::vector<bool> oldChannelUpdatesEnabledState(model->nChannels());
            size_t channelIdx = 0;
            for (std::vector<ChannelSpecs*>::iterator it =
                     model->begin(); it != model->end(); ++it, ++channelIdx)
            {
              oldChannelUpdatesEnabledState[channelIdx] =
                  (*it)->updatesEnabled();
              (*it)->setUpdatesEnabled(false);
            }
            p_orthoView->setPositionUm(newPos);
            channelIdx = 0;
            for (std::vector<ChannelSpecs*>::iterator it =
                     model->begin(); it != model->end(); ++it, ++channelIdx)
                (*it)->setUpdatesEnabled(
                    oldChannelUpdatesEnabledState[channelIdx]);

            bool oldViewUpdatesEnabledState = p_orthoView->updatesEnabled();
            p_orthoView->setUpdatesEnabled(false);
            for (std::vector<ChannelSpecs*>::iterator it =
                     model->begin(); it != model->end(); ++it)
            {
              ChannelSpecsOrthoViewRenderer *rend =
                  static_cast<ChannelSpecsOrthoViewRenderer*>(
                      (*it)->renderer(p_orthoView));
              if ((*it)->visible() && rend != NULL)
              {
                disconnect(
                    rend,
                    SIGNAL(cacheUpdated(
                               ChannelSpecsOrthoViewRenderer const*,int)),
                    p_orthoView,
                    SLOT(updatePlaneData(
                             ChannelSpecsOrthoViewRenderer const*,int)));
                rend->updateCache(_dimensions(1));
                connect(
                    rend,
                    SIGNAL(cacheUpdated(
                               ChannelSpecsOrthoViewRenderer const*,int)),
                    p_orthoView,
                    SLOT(updatePlaneData(
                             ChannelSpecsOrthoViewRenderer const*,int)));
              }
            }
            p_orthoView->setUpdatesEnabled(oldViewUpdatesEnabledState);
            p_orthoView->orthoViewPlane(_dimensions(1))->updateData();
            p_orthoView->update();
          }
        }

        if (_dragVertical && !_dragHorizontal)
        {
          newPos(_dimensions(0)) =
              mousePositionUm(e->x(), e->y())(_dimensions(0));

          MultiChannelModel *model = p_orthoView->model();
          if (model != NULL)
          {
            std::vector<bool> oldChannelUpdatesEnabledState(model->nChannels());
            size_t channelIdx = 0;
            for (std::vector<ChannelSpecs*>::iterator it =
                     model->begin(); it != model->end(); ++it, ++channelIdx)
            {
              oldChannelUpdatesEnabledState[channelIdx] =
                  (*it)->updatesEnabled();
              (*it)->setUpdatesEnabled(false);
            }
            p_orthoView->setPositionUm(newPos);
            channelIdx = 0;
            for (std::vector<ChannelSpecs*>::iterator it =
                     model->begin(); it != model->end(); ++it, ++channelIdx)
                (*it)->setUpdatesEnabled(
                    oldChannelUpdatesEnabledState[channelIdx]);

            bool oldViewUpdatesEnabledState = p_orthoView->updatesEnabled();
            p_orthoView->setUpdatesEnabled(false);
            for (std::vector<ChannelSpecs*>::iterator it =
                     model->begin(); it != model->end(); ++it)
            {
              ChannelSpecsOrthoViewRenderer *rend =
                  static_cast<ChannelSpecsOrthoViewRenderer*>(
                      (*it)->renderer(p_orthoView));
              if ((*it)->visible() && rend != NULL)
              {
                disconnect(
                    rend,
                    SIGNAL(cacheUpdated(
                               ChannelSpecsOrthoViewRenderer const*,int)),
                    p_orthoView,
                    SLOT(updatePlaneData(
                             ChannelSpecsOrthoViewRenderer const*,int)));
                rend->updateCache(_dimensions(0));
                connect(
                    rend,
                    SIGNAL(cacheUpdated(
                               ChannelSpecsOrthoViewRenderer const*,int)),
                    p_orthoView,
                    SLOT(updatePlaneData(
                             ChannelSpecsOrthoViewRenderer const*,int)));
              }
            }
            p_orthoView->setUpdatesEnabled(oldViewUpdatesEnabledState);
            p_orthoView->orthoViewPlane(_dimensions(0))->updateData();
            p_orthoView->update();
          }
        }

        if (_dragHorizontal && _dragVertical)
        {
          newPos(_dimensions(0)) =
              mousePositionUm(e->x(), e->y())(_dimensions(0));
          newPos(_dimensions(1)) =
              mousePositionUm(e->x(), e->y())(_dimensions(1));

          MultiChannelModel *model = p_orthoView->model();
          if (model != NULL)
          {
            std::vector<bool> oldChannelUpdatesEnabledState(model->nChannels());
            size_t channelIdx = 0;
            for (std::vector<ChannelSpecs*>::iterator it =
                     model->begin(); it != model->end(); ++it, ++channelIdx)
            {
              oldChannelUpdatesEnabledState[channelIdx] =
                  (*it)->updatesEnabled();
              (*it)->setUpdatesEnabled(false);
            }
            p_orthoView->setPositionUm(newPos);
            channelIdx = 0;
            for (std::vector<ChannelSpecs*>::iterator it =
                     model->begin(); it != model->end(); ++it, ++channelIdx)
                (*it)->setUpdatesEnabled(
                    oldChannelUpdatesEnabledState[channelIdx]);

            bool oldViewUpdatesEnabledState = p_orthoView->updatesEnabled();
            p_orthoView->setUpdatesEnabled(false);
            for (std::vector<ChannelSpecs*>::iterator it =
                     model->begin(); it != model->end(); ++it)
            {
              ChannelSpecsOrthoViewRenderer *rend =
                  static_cast<ChannelSpecsOrthoViewRenderer*>(
                      (*it)->renderer(p_orthoView));
              if ((*it)->visible() && rend != NULL)
              {
                disconnect(
                    rend,
                    SIGNAL(cacheUpdated(
                               ChannelSpecsOrthoViewRenderer const*,int)),
                    p_orthoView,
                    SLOT(updatePlaneData(
                             ChannelSpecsOrthoViewRenderer const*,int)));
                rend->updateCache(_dimensions(0));
                rend->updateCache(_dimensions(1));
                connect(
                    rend,
                    SIGNAL(cacheUpdated(
                               ChannelSpecsOrthoViewRenderer const*,int)),
                    p_orthoView,
                    SLOT(updatePlaneData(
                             ChannelSpecsOrthoViewRenderer const*,int)));
              }
            }
            p_orthoView->setUpdatesEnabled(oldViewUpdatesEnabledState);
            p_orthoView->orthoViewPlane(_dimensions(0))->updateData();
            p_orthoView->orthoViewPlane(_dimensions(1))->updateData();
            p_orthoView->update();
          }
        }
        _isDragging = true;
        e->accept();
        return;
      }
      else // MouseButton has been released ==> stop dragging
      {
        _dragHorizontal = false;
        _dragVertical = false;
        _isDragging = false;
      }
    }
  }

  // Independent of the mode emit the UserInteractionEvent
  if (p_orthoView != NULL && p_orthoView->model() != NULL &&
      p_orthoView->model()->selectedChannel() != NULL)
  {
    ChannelSpecsRenderer *renderer =
        p_orthoView->model()->selectedChannel()->renderer(p_orthoView);
    if (renderer != NULL)
    {
      OrthoViewUserInteractionEvent event(this, e, NULL);
      qApp->sendEvent(renderer, &event);
    }
#ifdef DEBUG
    else
        std::cerr << "No associated renderer for channel "
                  << p_orthoView->model()->selectedChannel()->name()
                  << " and this orthoview found" << std::endl;
#endif
  }
}

void OrthoViewPlane::mouseReleaseEvent(QMouseEvent* e)
{
  // If dragging release crosshair
  if (_isDragging && e->button() == Qt::LeftButton)
  {
    _dragHorizontal = false;
    _dragVertical = false;
    _isDragging = false;
    e->accept();
    return;
  }

  if (p_orthoView != NULL && p_orthoView->model() != NULL &&
      p_orthoView->model()->selectedChannel() != NULL)
  {
    ChannelSpecsRenderer *renderer =
        p_orthoView->model()->selectedChannel()->renderer(p_orthoView);
    if (renderer != NULL)
    {
      OrthoViewUserInteractionEvent event(this, e, NULL);
      qApp->sendEvent(renderer, &event);
    }
#ifdef DEBUG
    else
        std::cerr << "No associated renderer for channel "
                  << p_orthoView->model()->selectedChannel()->name()
                  << " and this orthoview found" << std::endl;
#endif
  }
}

void OrthoViewPlane::wheelEvent(QWheelEvent* e)
{
  if (p_orthoView != NULL && p_orthoView->model() != NULL &&
      p_orthoView->model()->selectedChannel() != NULL)
  {
    ChannelSpecsRenderer *renderer =
        p_orthoView->model()->selectedChannel()->renderer(p_orthoView);
    if (renderer != NULL)
    {
      OrthoViewUserInteractionEvent event(this, NULL, e);
      qApp->sendEvent(renderer, &event);
      if (event.isAccepted()) return;
    }
#ifdef DEBUG
    else
        std::cerr << "No associated renderer for channel "
                  << p_orthoView->model()->selectedChannel()->name()
                  << " and this orthoview found" << std::endl;
#endif
  }

  if (e->modifiers() == Qt::NoModifier) // Change slice
  {
    if (p_orthoView != NULL && p_orthoView->model() != NULL)
    {
      blitz::TinyVector<double,3> newPos(p_orthoView->positionUm());
      newPos(_orthogonalDimension) += ((e->delta() > 0) ? 1.0 : -1.0) *
          p_orthoView->model()->elementSizeUm()(_orthogonalDimension);
      p_orthoView->setPositionUm(newPos);

      bool oldViewUpdatesEnabledState = p_orthoView->updatesEnabled();
      p_orthoView->setUpdatesEnabled(false);
      for (std::vector<ChannelSpecs*>::iterator it =
               p_orthoView->model()->begin();
           it != p_orthoView->model()->end(); ++it)
      {
        ChannelSpecsOrthoViewRenderer *rend =
            static_cast<ChannelSpecsOrthoViewRenderer*>(
                (*it)->renderer(p_orthoView));
        if ((*it)->visible() && rend != NULL)
        {
          disconnect(
              rend,
              SIGNAL(cacheUpdated(ChannelSpecsOrthoViewRenderer const*,int)),
              p_orthoView,
              SLOT(updatePlaneData(ChannelSpecsOrthoViewRenderer const*,int)));
          rend->updateCache(_orthogonalDimension);
          connect(
              rend,
              SIGNAL(cacheUpdated(ChannelSpecsOrthoViewRenderer const*,int)),
              p_orthoView,
              SLOT(updatePlaneData(ChannelSpecsOrthoViewRenderer const*,int)));
        }
      }
      p_orthoView->setUpdatesEnabled(oldViewUpdatesEnabledState);
      p_orthoView->orthoViewPlane(_orthogonalDimension)->updateData();
      p_orthoView->update();
      e->accept();
      return;
    }
  }

  if (e->modifiers().testFlag(Qt::ControlModifier)) // Zoom
  {
    if (p_orthoView != NULL)
    {
      // Avoid zooming to zero size
      if (p_orthoView->zoom() + ((e->delta() > 0) ? 0.1 : -0.1) > 0.001)
          p_orthoView->setZoom(
              p_orthoView->zoom() + ((e->delta() > 0) ? 0.1 : -0.1));
      e->accept();
      return;
    }
  }

}
