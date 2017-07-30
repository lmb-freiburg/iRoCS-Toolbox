/**************************************************************************
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

#include "FloatChannelOrthoviewRenderer.hh"

#include <QtGui/QPainter>

#include <libMultiChannelModel/FloatChannel.hh>
#include "OrthoviewWidget.hh"
#include "OrthoviewPlane.hh"

namespace iRoCS {

  FloatChannelOrthoviewRenderer::FloatChannelOrthoviewRenderer(
    OrthoviewWidget &view, FloatChannel &channel)
    : ChannelOrthoviewRenderer(view, channel),
    FloatColormapObserver(channel.colormap()) {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<FloatChannelOrthoviewRenderer@" << this
      << "::FloatChannelOrthoviewRenderer view=\"" << &view
      << "\" channel=\"" << &channel << "\" />" << std::endl;
#endif
  }

  FloatChannelOrthoviewRenderer::~FloatChannelOrthoviewRenderer() {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<FloatChannelOrthoviewRenderer@" << this
      << "::~FloatChannelOrthoviewRenderer />" << std::endl;
#endif
  }

  Observable const &FloatChannelOrthoviewRenderer::observable() const {
    return _channel;
  }

  Observable &FloatChannelOrthoviewRenderer::observable() {
    return _channel;
  }

  bool FloatChannelOrthoviewRenderer::rendersImageData() const {
    return true;
  }

  bool FloatChannelOrthoviewRenderer::rendersVectorData() const {
    return false;
  }

  void FloatChannelOrthoviewRenderer::updateName() {}

  void FloatChannelOrthoviewRenderer::updateFilename() {}

  void FloatChannelOrthoviewRenderer::updateAlpha() {
    if (_channel.visible()) orthoview().update();
  }

  void FloatChannelOrthoviewRenderer::updateVisibilityFlag() {
    orthoview().update();
  }

  void FloatChannelOrthoviewRenderer::updateModifiedFlag() {}

  void FloatChannelOrthoviewRenderer::updateElementSizeUm() {
    if (_channel.visible()) orthoview().update();
  }

  void FloatChannelOrthoviewRenderer::updateTransformation() {
    if (_channel.visible()) orthoview().update();
  }

  void FloatChannelOrthoviewRenderer::updateIndexRange() {
    if (_channel.visible()) orthoview().update();
  }

  void FloatChannelOrthoviewRenderer::updateStartColor() {
    if (_channel.visible()) orthoview().update();
  }

  void FloatChannelOrthoviewRenderer::updateEndColor() {
    if (_channel.visible()) orthoview().update();
  }

  void FloatChannelOrthoviewRenderer::updateIntermediateColor(double) {
    if (_channel.visible()) orthoview().update();
  }

  void FloatChannelOrthoviewRenderer::updateMoveIntermediateColor(
    double, double) {
    if (_channel.visible()) orthoview().update();
  }

  void FloatChannelOrthoviewRenderer::updateRemoveIntermediateColor(double) {
    if (_channel.visible()) orthoview().update();
  }

  void FloatChannelOrthoviewRenderer::updateClearIntermediateColors() {
    if (_channel.visible()) orthoview().update();
  }

  void FloatChannelOrthoviewRenderer::updateMonochromeColor() {
    if (_channel.visible()) orthoview().update();
  }

  void FloatChannelOrthoviewRenderer::updateRangeIndicator() {
    if (_channel.visible()) orthoview().update();
  }

  void FloatChannelOrthoviewRenderer::updateGamma() {
    if (_channel.visible()) orthoview().update();
  }

  void FloatChannelOrthoviewRenderer::updateColormap() {
    if (_channel.visible()) orthoview().update();
  }

  void FloatChannelOrthoviewRenderer::updateData() {
    if (_channel.visible()) orthoview().update();
  }

  void FloatChannelOrthoviewRenderer::renderImageData(
    int orthogonalDimension, QRegion const &region) {
    FloatChannel &chan = static_cast<FloatChannel&>(_channel);

    if (!chan.visible()) return;

#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<FloatChannelOrthoviewRenderer@" << this
      << "::renderImageData orthogonalDimension=\""
      << orthogonalDimension << "\">" << std::endl;
#endif

    OrthoviewWidget &ov = _orthoview;
    blitz::TinyVector<double, 3> const &pixelSizeUm = ov.pixelSizeUm();
    blitz::TinyVector<double, 3> const &centerPosUm = ov.centerPositionUm();
    double mag = ov.magnification();
    OrthoviewPlane *op = ov.plane(orthogonalDimension);
    QRgb *imageData = op->imageData();
    blitz::TinyVector<int, 2> const &dims = op->planeDimensions();

    blitz::TinyMatrix<double, 4, 4> const &trafo = chan.transformation();
    bool identityTrafo =
      (blitz::all(
        trafo == atb::traits< blitz::TinyMatrix<double, 4, 4> >::one));
    blitz::TinyVector<double, 3> const &elSize = chan.elementSizeUm();
    atb::Array<float, 3> const &data = chan.data();
    double alpha = chan.alpha();
    double scaleX = pixelSizeUm(dims(1)) / mag;
    double scaleY = pixelSizeUm(dims(0)) / mag;
    double oX = centerPosUm(dims(1));
    double oY = centerPosUm(dims(0));

    // Clip the paint region with the channel bounding box
    blitz::TinyVector<double, 3> channelLbUm(chan.boundingBoxLowerBoundUm());
    blitz::TinyVector<double, 3> channelUbUm(chan.boundingBoxUpperBoundUm());
    QPointF channelLbPx(op->absoluteUmToPx(channelLbUm - 1.0));
    QPointF channelUbPx(op->absoluteUmToPx(channelUbUm + 1.0));
    QRect channelRectPx(
      QPoint(std::floor(channelLbPx.rx()), std::floor(channelLbPx.ry())),
      QPoint(std::ceil(channelUbPx.rx()), std::ceil(channelUbPx.ry())));
    QRegion clippedRegion = region & channelRectPx;

    for (int r = 0; r < clippedRegion.rects().size(); ++r) {
      blitz::TinyVector<int, 2> pos, shape;
      clippedRegion.rects()[r].getRect(&pos(1), &pos(0), &shape(1), &shape(0));
      double dxCanvas = pos(1) - op->width() / 2.0;
      double dyCanvas = pos(0) - op->height() / 2.0;
      blitz::TinyVector<double, 3> paintLbPx, paintUbPx;
      atb::Interpolator<float, 3> *ip;
      if (ov.interpolationEnabled()) {
        ip = new atb::LinearInterpolator<float, 3>(atb::ValueBT, 0.0f);
        paintLbPx = -1.0, -1.0, -1.0;
        paintUbPx = blitz::TinyVector<double, 3>(data.shape());
      }
      else {
        ip = new atb::NearestInterpolator<float, 3>(atb::ValueBT, 0.0f);
        paintLbPx = -0.5, -0.5, -0.5;
        paintUbPx = blitz::TinyVector<double, 3>(data.shape()) - 0.5;
      }
#ifdef _OPENMP
#pragma omp parallel for
#endif
      for (int y = 0; y < shape(0); ++y) {
        blitz::TinyVector<double, 4> posUmHom;
        posUmHom(orthogonalDimension) =
          ov.crossPositionUm()(orthogonalDimension);
        posUmHom(3) = 1.0;
        double &yUm = posUmHom(dims(0));
        double &xUm = posUmHom(dims(1));
        yUm = (y + dyCanvas) * scaleY + oY;
        QRgb *currentPixel =
          imageData + ((y + pos(0)) * op->width() + pos(1));
        for (int x = 0; x < shape(1); ++x) {
          xUm = (x + dxCanvas) * scaleX + oX;
          blitz::TinyVector<double, 3> posPx;
          if (identityTrafo)
            posPx = blitz::TinyVector<double, 3>(
              posUmHom(0), posUmHom(1), posUmHom(2)) / elSize;
          else posPx = atb::homogeneousToEuclidean(trafo * posUmHom) / elSize;
          if (blitz::any(posPx < paintLbPx || posPx > paintUbPx))
            currentPixel++;
          else {
            FloatColormap::ColorT col(_colormap.color(ip->get(data, posPx)));
            *currentPixel = qRgb(
              std::min(255, static_cast<int>(
                qRed(*currentPixel) + alpha * col(0))),
              std::min(255, static_cast<int>(
                qGreen(*currentPixel) + alpha * col(1))),
              std::min(255, static_cast<int>(
                qBlue(*currentPixel) + alpha * col(2))));
            currentPixel++;
          }
        }
      }
      delete ip;
    }

#ifdef DEBUG_VERBOSE_XML
    std::cerr << "</FloatChannelOrthoviewRenderer@" << this
      << "::renderImageData>" << std::endl;
#endif
  }

  void FloatChannelOrthoviewRenderer::renderVectorData(int, QPainter &) {
    // No vectorial data to render for FloatChannels
  }

  FloatChannelOrthoviewRenderer *FloatChannelOrthoviewRenderer::instance(
    OrthoviewWidget &view, FloatChannel &channel) {
    // First check whether an Observer for this channel/view combination
    // already exists. If so return the pointer to the existing Observer.
    for (std::set<ChannelObserver*>::const_iterator it =
      channel.observers().begin(); it != channel.observers().end(); ++it) {
      FloatChannelOrthoviewRenderer *renderer =
        dynamic_cast<FloatChannelOrthoviewRenderer*>(*it);
      if (renderer != NULL &&
        &dynamic_cast<Channel&>(renderer->channel()) == &channel)
        return renderer;
    }
    return new FloatChannelOrthoviewRenderer(view, channel);
  }

}
