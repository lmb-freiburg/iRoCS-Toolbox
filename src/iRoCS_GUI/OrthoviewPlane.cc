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

#include "OrthoviewPlane.hh"
#include "OrthoviewWidget.hh"
#include "ChannelOrthoviewRenderer.hh"

#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>
#include <QtCore/QCoreApplication>

#include <cstring>

#ifdef HAVE_BLITZ_V9
#include <blitz/tinyvec-et.h>
#endif

namespace iRoCS {

  OrthoviewPlane::OrthoviewPlane(
    int orthogonalDimension, OrthoviewWidget *orthoview, QWidget *parent,
    Qt::WindowFlags f)
    : QWidget(parent, f), p_orthoview(orthoview),
    _orthogonalDimension(orthogonalDimension), _planeDimensions(0),
    p_imageData(NULL), _dragState(None),
    _previousGlobalMousePositionPx(0, 0) {
    switch (_orthogonalDimension) {
      case 0:
        _planeDimensions(0) = 1;
        _planeDimensions(1) = 2;
        break;
      case 1:
        _planeDimensions(0) = 0;
        _planeDimensions(1) = 2;
        break;
      case 2:
        _planeDimensions(0) = 1;
        _planeDimensions(1) = 0;
        break;
      default:
        std::cerr << __FILE__ << ":" << __LINE__ << ": orthogonalDimension "
          << orthogonalDimension << " is out-of-bounds." << std::endl;
        return;
    }
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    setMouseTracking(true);
  }

  OrthoviewPlane::~OrthoviewPlane() {
    delete[] p_imageData;
    p_imageData = NULL;
  }

  blitz::TinyVector<int, 2> const &OrthoviewPlane::planeDimensions() const {
    return _planeDimensions;
  }

  QRgb const *OrthoviewPlane::imageData() const {
    return p_imageData;
  }

  QRgb *OrthoviewPlane::imageData() {
    return p_imageData;
  }

  QPointF OrthoviewPlane::absoluteUmToPx(
    blitz::TinyVector<double, 3> const &positionUm) const {
    return QPointF(
      (positionUm(_planeDimensions(1)) -
        p_orthoview->centerPositionUm()(_planeDimensions(1))) *
      p_orthoview->magnification() /
      p_orthoview->pixelSizeUm()(_planeDimensions(1)) +
      static_cast<double>(width()) / 2.0,
      (positionUm(_planeDimensions(0)) -
        p_orthoview->centerPositionUm()(_planeDimensions(0))) *
      p_orthoview->magnification() /
      p_orthoview->pixelSizeUm()(_planeDimensions(0)) +
      static_cast<double>(height()) / 2.0);
  }

  QPointF OrthoviewPlane::relativeUmToPx(
    blitz::TinyVector<double, 3> const &offsetUm) const {
    return QPointF(
      offsetUm(_planeDimensions(1)) *
      p_orthoview->magnification() /
      p_orthoview->pixelSizeUm()(_planeDimensions(1)),
      offsetUm(_planeDimensions(0)) *
      p_orthoview->magnification() /
      p_orthoview->pixelSizeUm()(_planeDimensions(0)));
  }

  blitz::TinyVector<double, 3> OrthoviewPlane::absolutePxToUm(
    int x, int y) const {
    blitz::TinyVector<double, 3> posUm;
    posUm(_orthogonalDimension) = p_orthoview->crossPositionUm()(
      _orthogonalDimension);
    posUm(_planeDimensions(0)) =
      (static_cast<double>(y) - static_cast<double>(height()) / 2.0) *
      p_orthoview->pixelSizeUm()(_planeDimensions(0)) /
      p_orthoview->magnification() +
      p_orthoview->centerPositionUm()(_planeDimensions(0));
    posUm(_planeDimensions(1)) =
      (static_cast<double>(x) - static_cast<double>(width()) / 2.0) *
      p_orthoview->pixelSizeUm()(_planeDimensions(1)) /
      p_orthoview->magnification() +
      p_orthoview->centerPositionUm()(_planeDimensions(1));
    return posUm;
  }

  blitz::TinyVector<double, 3> OrthoviewPlane::relativePxToUm(
    int dx, int dy) const {
    blitz::TinyVector<double, 3> relativePosUm;
    relativePosUm(_orthogonalDimension) = 0.0;
    relativePosUm(_planeDimensions(0)) =
      static_cast<double>(dy) *
      p_orthoview->pixelSizeUm()(_planeDimensions(0)) /
      p_orthoview->magnification();
    relativePosUm(_planeDimensions(1)) =
      static_cast<double>(dx) *
      p_orthoview->pixelSizeUm()(_planeDimensions(1)) /
      p_orthoview->magnification();
    return relativePosUm;
  }

  void OrthoviewPlane::resizeEvent(QResizeEvent *) {
    delete[] p_imageData;
    p_imageData = NULL;
    update();
  }

  void OrthoviewPlane::paintEvent(QPaintEvent *e) {
    if (p_orthoview == NULL) return;

#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<OrthoviewPlane@" << this << "::paintEvent e=\"" << e
      << "\" p_orthoview = \"" << p_orthoview
      << "\" orthogonalDimension=\"" << _orthogonalDimension << "\">"
      << std::endl;
#endif

    if (p_imageData == NULL)
      p_imageData = new QRgb[width() * height()];
    std::memset(p_imageData, 0, width() * height() * sizeof(QRgb));

    QPainter painter(this);
    painter.setClipRegion(e->region());

    for (std::set<OrthoviewRenderer*>::iterator it =
      p_orthoview->renderers().begin(); it !=
      p_orthoview->renderers().end(); ++it)
      (*it)->renderImageData(_orthogonalDimension, painter.clipRegion());

    QImage image(
      reinterpret_cast<unsigned char*>(p_imageData), width(), height(),
      QImage::Format_ARGB32_Premultiplied);

    for (int r = 0; r < painter.clipRegion().rects().size(); ++r)
      painter.drawImage(
        painter.clipRegion().rects()[r], image,
        painter.clipRegion().rects()[r]);

    for (std::set<OrthoviewRenderer*>::iterator it =
      p_orthoview->renderers().begin(); it !=
      p_orthoview->renderers().end(); ++it)
      (*it)->renderVectorData(_orthogonalDimension, painter);

    if (p_orthoview != NULL) {
      blitz::TinyVector<double, 2> crossPositionPx(
        (p_orthoview->crossPositionUm()(_planeDimensions(0)) -
          p_orthoview->centerPositionUm()(_planeDimensions(0))) *
        p_orthoview->magnification() /
        p_orthoview->pixelSizeUm()(_planeDimensions(0)) + height() / 2.0,
        (p_orthoview->crossPositionUm()(_planeDimensions(1)) -
          p_orthoview->centerPositionUm()(_planeDimensions(1))) *
        p_orthoview->magnification() /
        p_orthoview->pixelSizeUm()(_planeDimensions(1)) + width() / 2.0);

      QPen pen(Qt::red);
      pen.setWidth(0);
      painter.save();
      painter.setPen(pen);
      painter.drawLine(0, crossPositionPx(0), width(), crossPositionPx(0));
      painter.drawLine(crossPositionPx(1), 0, crossPositionPx(1), height());
      painter.restore();

      if (_orthogonalDimension == 0 && p_orthoview->scalebarVisible()) {
        double widgetWidthUm =
          static_cast<double>(width()) *
          p_orthoview->pixelSizeUm()(_planeDimensions(1)) /
          p_orthoview->magnification();
        double scalebarLengthUm = 1;
        while (scalebarLengthUm > widgetWidthUm / 2.0)
          scalebarLengthUm /= 10;
        while (true) {
          scalebarLengthUm *= 5.0;
          if (scalebarLengthUm > widgetWidthUm / 2.0) {
            scalebarLengthUm /= 5.0;
            break;
          }
          scalebarLengthUm *= 2;
          if (scalebarLengthUm > widgetWidthUm / 2.0) {
            scalebarLengthUm /= 2.0;
            break;
          }
        }
        double scalebarLengthPx =
          scalebarLengthUm * p_orthoview->magnification() /
          p_orthoview->pixelSizeUm()(_planeDimensions(1));
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::white);
        painter.drawRect(10, height() - 12, scalebarLengthPx, 5);
        painter.setPen(Qt::SolidLine);
        painter.setPen(Qt::white);
        painter.drawText(
          QRect(10, height() - 25, scalebarLengthPx, 10),
          Qt::AlignCenter, tr("%1 µm").arg(scalebarLengthUm));
      }
    }
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "</OrthoviewPlane@" << this << "::paintEvent>" << std::endl;
#endif
  }

  void OrthoviewPlane::mouseMoveEvent(QMouseEvent *e) {
    e->ignore();
    if (p_orthoview == NULL) return;

    blitz::TinyVector<double, 2> crossPositionPx(
      (p_orthoview->crossPositionUm()(_planeDimensions(0)) -
        p_orthoview->centerPositionUm()(_planeDimensions(0))) *
      p_orthoview->magnification() /
      p_orthoview->pixelSizeUm()(_planeDimensions(0)) + height() / 2.0,
      (p_orthoview->crossPositionUm()(_planeDimensions(1)) -
        p_orthoview->centerPositionUm()(_planeDimensions(1))) *
      p_orthoview->magnification() /
      p_orthoview->pixelSizeUm()(_planeDimensions(1)) + width() / 2.0);

    // Setup Cursor shape according to cross (independent of
    // selection/dragging mode)
    if (std::abs(e->x() - crossPositionPx(1)) < 3) {
      if (std::abs(e->y() - crossPositionPx(0)) < 3)
        setCursor(Qt::SizeAllCursor);
      else setCursor(Qt::SplitHCursor);
    }
    else {
      if (std::abs(e->y() - crossPositionPx(0)) < 3)
        setCursor(Qt::SplitVCursor);
      else setCursor(Qt::CrossCursor);
    }

    emit mousePositionChanged(_orthogonalDimension, e->pos());

    if (!e->buttons().testFlag(Qt::LeftButton)) _dragState = None;

    if (_dragState != None) {
      switch (_dragState) {
        case View:
        {
          QPoint dPos = e->globalPos() - _previousGlobalMousePositionPx;
          p_orthoview->setCenterPositionUm(
            blitz::TinyVector<double, 3>(
              p_orthoview->centerPositionUm() -
              relativePxToUm(dPos.x(), dPos.y())));
          break;
        }
        case Horizontal:
        {
          blitz::TinyVector<double, 3> mousePositionUm;
          mousePositionUm(_orthogonalDimension) =
            p_orthoview->crossPositionUm()(_orthogonalDimension);
          mousePositionUm(_planeDimensions(0)) =
            p_orthoview->crossPositionUm()(_planeDimensions(0));
          mousePositionUm(_planeDimensions(1)) =
            (static_cast<double>(e->x()) -
              static_cast<double>(width()) / 2.0) *
            p_orthoview->pixelSizeUm()(_planeDimensions(1)) /
            p_orthoview->magnification() +
            p_orthoview->centerPositionUm()(_planeDimensions(1));
          p_orthoview->setCrossPositionUm(mousePositionUm);
          break;
        }
        case Vertical:
        {
          blitz::TinyVector<double, 3> mousePositionUm;
          mousePositionUm(_orthogonalDimension) =
            p_orthoview->crossPositionUm()(_orthogonalDimension);
          mousePositionUm(_planeDimensions(0)) =
            (static_cast<double>(e->y()) -
              static_cast<double>(height()) / 2.0) *
            p_orthoview->pixelSizeUm()(_planeDimensions(0)) /
            p_orthoview->magnification() +
            p_orthoview->centerPositionUm()(_planeDimensions(0));
          mousePositionUm(_planeDimensions(1)) =
            p_orthoview->crossPositionUm()(_planeDimensions(1));
          p_orthoview->setCrossPositionUm(mousePositionUm);
          break;
        }
        case Cross:
        {
          p_orthoview->setCrossPositionUm(absolutePxToUm(e->x(), e->y()));
          break;
        }
        default:
          break;
      }
      e->accept();
      _previousGlobalMousePositionPx = e->globalPos();
      return;
    }

    for (std::set<OrthoviewRenderer*>::iterator it =
      p_orthoview->renderers().begin();
      it != p_orthoview->renderers().end() && !e->isAccepted(); ++it)
      qApp->sendEvent(*it, e);
  }

  void OrthoviewPlane::mousePressEvent(QMouseEvent *e) {
    e->ignore();
    if (p_orthoview == NULL) return;

    // First check whether the event is handled by a channel
    for (std::set<OrthoviewRenderer*>::iterator it =
      p_orthoview->renderers().begin();
      it != p_orthoview->renderers().end() && !e->isAccepted(); ++it)
      qApp->sendEvent(*it, e);
    if (e->isAccepted()) return;

    // No channel wanted the event, so the orthoview handles it itself
    blitz::TinyVector<double, 2> crossPositionPx(
      (p_orthoview->crossPositionUm()(_planeDimensions(0)) -
        p_orthoview->centerPositionUm()(_planeDimensions(0))) *
      p_orthoview->magnification() /
      p_orthoview->pixelSizeUm()(_planeDimensions(0)) + height() / 2.0,
      (p_orthoview->crossPositionUm()(_planeDimensions(1)) -
        p_orthoview->centerPositionUm()(_planeDimensions(1))) *
      p_orthoview->magnification() /
      p_orthoview->pixelSizeUm()(_planeDimensions(1)) + width() / 2.0);

    // Setup Cursor shape according to cross (independent of
    // selection/dragging mode)
    if (std::abs(e->x() - crossPositionPx(1)) < 3) {
      if (std::abs(e->y() - crossPositionPx(0)) < 3)
        _dragState = Cross;
      else _dragState = Horizontal;
    }
    else {
      if (std::abs(e->y() - crossPositionPx(0)) < 3)
        _dragState = Vertical;
      else _dragState = View;
    }
    _previousGlobalMousePositionPx = e->globalPos();
    e->accept();
  }

  void OrthoviewPlane::mouseReleaseEvent(QMouseEvent *e) {
    e->ignore();
    if (p_orthoview == NULL) return;

    if (_dragState != None) {
      _dragState = None;
      e->accept();
      return;
    }

    for (std::set<OrthoviewRenderer*>::iterator it =
      p_orthoview->renderers().begin();
      it != p_orthoview->renderers().end() && !e->isAccepted(); ++it)
      qApp->sendEvent(*it, e);
  }

  void OrthoviewPlane::mouseDoubleClickEvent(QMouseEvent *e) {
    e->ignore();
    if (p_orthoview == NULL) return;

    // First check whether the event is handled by a channel
    for (std::set<OrthoviewRenderer*>::iterator it =
      p_orthoview->renderers().begin();
      it != p_orthoview->renderers().end() && !e->isAccepted(); ++it)
      qApp->sendEvent(*it, e);
    if (e->isAccepted()) return;

    // No channel wanted the event, so the orthoview handles it itself
    if (e->modifiers() == Qt::NoModifier) // Change slice
    {
      p_orthoview->setCrossPositionUm(absolutePxToUm(e->x(), e->y()));
      e->accept();
      return;
    }
  }

  void OrthoviewPlane::wheelEvent(QWheelEvent *e) {
    e->ignore();
    if (p_orthoview == NULL) return;

    // First check whether the event is handled by a channel
    for (std::set<OrthoviewRenderer*>::iterator it =
      p_orthoview->renderers().begin();
      it != p_orthoview->renderers().end() && !e->isAccepted(); ++it)
      qApp->sendEvent(*it, e);
    if (e->isAccepted()) return;

    // No channel wanted the event, so the orthoview handles it itself
    if (e->modifiers() == Qt::NoModifier) // Change slice
    {
      blitz::TinyVector<double, 3> pos(p_orthoview->crossPositionUm());
      pos(_orthogonalDimension) += ((e->delta() > 0) ? 1.0 : -1.0) *
        p_orthoview->pixelSizeUm()(_orthogonalDimension);
      p_orthoview->setCrossPositionUm(pos);
      e->accept();
      return;
    }

    if (e->modifiers().testFlag(Qt::ControlModifier)) // Zoom
    {
      double magnification = p_orthoview->magnification() *
        ((e->delta() > 0) ? 1.1 : 0.9);

      blitz::TinyVector<double, 3> oldMousePositionUm(
        absolutePxToUm(e->x(), e->y()));

      if (magnification >= 0.01) p_orthoview->setMagnification(magnification);

      blitz::TinyVector<double, 3> newMousePositionUm(
        absolutePxToUm(e->x(), e->y()));

      blitz::TinyVector<double, 3> centerPositionUm(
        p_orthoview->centerPositionUm() +
        (oldMousePositionUm - newMousePositionUm));
      p_orthoview->setCenterPositionUm(centerPositionUm);

      e->accept();
      return;
    }
  }

}
