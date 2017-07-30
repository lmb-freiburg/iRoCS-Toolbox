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

#include "FloatColormapBarWidget.hh"
#include <libMultiChannelModel/FloatColormap.hh>
#include "DoubleControlElement.hh"
#include "ColorControlElement.hh"

#include <QtCore/QResource>
#include <QtGui/QMouseEvent>
#include <QtGui/QDialog>
#include <QtGui/QBoxLayout>
#include <QtGui/QFrame>
#include <QtGui/QToolButton>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QLabel>
#include <QtGui/QSpinBox>
#include <QtGui/QMenu>
#include <QtGui/QAction>
#include <QtGui/QPainter>

namespace iRoCS {

  /*-----------------------------------------------------------------------
   *  FloatColormapBarWidget
   *-----------------------------------------------------------------------*/
  FloatColormapBarWidget::FloatColormapBarWidget(
    FloatColormap &colormap, QWidget *parent)
    : QWidget(parent), FloatColormapObserver(colormap),
    _draggedIntermediateColor(-1.0) {
    resize(sizeHint());
    setFixedHeight(sizeHint().height());
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  }

  FloatColormapBarWidget::~FloatColormapBarWidget() {}

  QSize FloatColormapBarWidget::sizeHint() const {
    return QSize(256, 30);
  }

  void FloatColormapBarWidget::updateIndexRange() {
    update();
  }

  void FloatColormapBarWidget::updateStartColor() {
    update();
  }

  void FloatColormapBarWidget::updateEndColor() {
    update();
  }

  void FloatColormapBarWidget::updateIntermediateColor(double) {
    update();
  }

  void FloatColormapBarWidget::updateMoveIntermediateColor(double, double) {
    update();
  }

  void FloatColormapBarWidget::updateRemoveIntermediateColor(double) {
    update();
  }

  void FloatColormapBarWidget::updateClearIntermediateColors() {
    update();
  }

  void FloatColormapBarWidget::updateMonochromeColor() {
    update();
  }

  void FloatColormapBarWidget::updateRangeIndicator() {
    update();
  }

  void FloatColormapBarWidget::updateGamma() {
    update();
  }

  void FloatColormapBarWidget::updateColormap() {
    update();
  }

  void FloatColormapBarWidget::resizeEvent(QResizeEvent *) {
    update();
  }

  void FloatColormapBarWidget::mousePressEvent(QMouseEvent *e) {
    e->ignore();
    if (e->button() == Qt::LeftButton) {
      if (e->pos().y() >= 15) // Possible intermediate color drag initiation
      {
        if (_colormap.intermediateColors().size() > 0) {
          std::map<double, FloatColormap::ColorT>::const_iterator it =
            _colormap.intermediateColors().begin();
          float posPx =
            std::pow(it->first, _colormap.gamma()) * (width() - 1);
          float minDist = std::abs(posPx - static_cast<float>(e->pos().x()));
          double draggedPos = it->first;
          ++it;
          for (; it != _colormap.intermediateColors().end(); ++it) {
            float posPx =
              std::pow(it->first, _colormap.gamma()) * (width() - 1);
            float dist = std::abs(posPx - static_cast<float>(e->pos().x()));
            if (dist < minDist) {
              minDist = dist;
              draggedPos = it->first;
            }
          }
          if (minDist < 5) {
            _draggedIntermediateColor = draggedPos;
            e->accept();
            return;
          }
        }
      }
    }
    if (e->button() == Qt::RightButton) {
      if (e->pos().y() >= 15) // Possible intermediate color context request
      {
        if (_colormap.intermediateColors().size() > 0) {
          std::map<double, FloatColormap::ColorT>::const_iterator it =
            _colormap.intermediateColors().begin();
          float posPx =
            std::pow(it->first, _colormap.gamma()) * (width() - 1);
          float minDist = std::abs(posPx - static_cast<float>(e->pos().x()));
          double selectedPos = it->first;
          ++it;
          for (; it != _colormap.intermediateColors().end(); ++it) {
            float posPx =
              std::pow(it->first, _colormap.gamma()) * (width() - 1);
            float dist = std::abs(posPx - static_cast<float>(e->pos().x()));
            if (dist < minDist) {
              minDist = dist;
              selectedPos = it->first;
            }
          }
          if (minDist < 5) {
            QDialog intermediateColorEditor(this, Qt::Popup);
            QHBoxLayout *layout = new QHBoxLayout(&intermediateColorEditor);
            layout->setContentsMargins(0, 0, 0, 0);
            QFrame *frame = new QFrame;
            frame->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
            QHBoxLayout *frameLayout = new QHBoxLayout(frame);
            frameLayout->setContentsMargins(0, 0, 0, 0);
            QToolButton *removeButton = new QToolButton;
            removeButton->setIcon(
              QIcon::fromTheme("edit-delete", QIcon(":/edit-delete.png")));
            frameLayout->addWidget(removeButton);
            connect(removeButton, SIGNAL(clicked()),
              &intermediateColorEditor, SLOT(accept()));
            DoubleControlElement *positionControl =
              new DoubleControlElement(tr("Pos:"));
            positionControl->setRange(0.000001, 0.999999);
            positionControl->setValue(selectedPos);
            positionControl->setDecimals(6);
            positionControl->setSingleStep(0.01);
            frameLayout->addWidget(positionControl);
            ColorControlElement *colorControl =
              new ColorControlElement(tr("Color:"));
            FloatColormap::ColorT oldColor =
              _colormap.intermediateColors().find(selectedPos)->second;
            colorControl->setValue(oldColor);
            frameLayout->addWidget(colorControl);
            layout->addWidget(frame);

            // This is a workaround to ensure that the geometry calculation
            // is done
            intermediateColorEditor.move(-5000, -5000);
            intermediateColorEditor.show();

            // Ensure that the whole editor widget is visible on the current
            // screen
            QPoint editorPosition(QCursor::pos());
            QRect screenGeometry(
              QApplication::desktop()->screenGeometry(
                QApplication::desktop()->screenNumber(this)));
            if (editorPosition.x() - screenGeometry.x() +
              intermediateColorEditor.width() >
              screenGeometry.width())
              editorPosition.rx() = screenGeometry.x() +
              screenGeometry.width() -
              intermediateColorEditor.width() - 1;
            if (editorPosition.y() - screenGeometry.y() +
              intermediateColorEditor.height() >
              screenGeometry.height())
              editorPosition.ry() = screenGeometry.y() +
              screenGeometry.height() -
              intermediateColorEditor.height() - 1;
            intermediateColorEditor.move(
              editorPosition.x(), editorPosition.y());

            int res = intermediateColorEditor.exec();
            if (res == QDialog::Accepted)
              _colormap.removeIntermediateColor(selectedPos);
            else {
              if (blitz::any(oldColor != colorControl->value()))
                _colormap.setIntermediateColor(
                  selectedPos, colorControl->value());
              if (positionControl->value() != selectedPos)
                _colormap.moveIntermediateColor(
                  selectedPos, positionControl->value());
            }
            e->accept();
            return;
          }
        }
      }

      // General context menu requested
      QMenu menu;
      QAction *addIntermediateColorAction =
        new QAction(tr("Add intermediate color"), &menu);
      menu.addAction(addIntermediateColorAction);
      QAction *clearIntermediateColorsAction =
        new QAction(tr("Clear intermediate colors"), &menu);
      menu.addAction(clearIntermediateColorsAction);
      QAction *res = menu.exec(QCursor::pos());
      if (res == addIntermediateColorAction) {
        double pos = std::pow(
          static_cast<double>(e->pos().x()) /
          static_cast<double>(width() - 1), 1.0 / _colormap.gamma());
        double scale = _colormap.endIndex() - _colormap.startIndex() + 1;
        double offset = _colormap.startIndex();
        int index = static_cast<int>(
          std::floor(static_cast<double>(e->pos().x()) /
            static_cast<double>(width() - 1) * scale + offset));
        _colormap.setIntermediateColor(pos, _colormap.color(index));
      }
      else if (res == clearIntermediateColorsAction)
        _colormap.clearIntermediateColors();
      e->accept();
      return;
    }
  }

  void FloatColormapBarWidget::mouseReleaseEvent(QMouseEvent *e) {
    e->ignore();
    _draggedIntermediateColor = -1.0;
    e->accept();
  }

  void FloatColormapBarWidget::mouseMoveEvent(QMouseEvent *e) {
    e->ignore();
    if (_draggedIntermediateColor != -1.0) {
      double oldPos = _draggedIntermediateColor;
      double newPos = std::pow(
        static_cast<double>(e->pos().x()) / static_cast<double>(width() - 1),
        1.0 / _colormap.gamma());
      _draggedIntermediateColor =
        _colormap.moveIntermediateColor(oldPos, newPos);
      if (_draggedIntermediateColor != oldPos) update();
      e->accept();
      return;
    }
  }

  void FloatColormapBarWidget::mouseDoubleClickEvent(QMouseEvent *e) {
    e->ignore();
    double pos = std::pow(
      static_cast<double>(e->pos().x()) /
      static_cast<double>(width() - 1), 1.0 / _colormap.gamma());
    double scale = _colormap.endIndex() - _colormap.startIndex() + 1;
    double offset = _colormap.startIndex();
    int index = static_cast<int>(
      std::floor(static_cast<double>(e->pos().x()) /
        static_cast<double>(width() - 1) * scale + offset));
    _colormap.setIntermediateColor(pos, _colormap.color(index));
    e->accept();
  }

  void FloatColormapBarWidget::paintEvent(QPaintEvent*) {
    QImage colorbar(QSize(width(), 1), QImage::Format_RGB32);
    for (int x = 0; x < width(); ++x) {
      FloatColormap::ColorT color = _colormap.color(
        static_cast<double>(x) / static_cast<double>(width() - 1) *
        (_colormap.endIndex() - _colormap.startIndex()) +
        _colormap.startIndex());
      colorbar.setPixel(x, 0, QColor(color(0), color(1), color(2)).rgb());
    }
    QPainter painter(this);
    painter.drawImage(
      QRect(0, 5, width(), 20), colorbar, QRect(0, 0, width(), 1));

    QPalette::ColorGroup group =
      (isEnabled()) ? QPalette::Active : QPalette::Inactive;

    for (std::map<double, FloatColormap::ColorT>::const_iterator it =
      _colormap.intermediateColors().begin();
      it != _colormap.intermediateColors().end(); ++it) {
      float posPx = std::pow(it->first, _colormap.gamma()) * (width() - 1);

      painter.setPen(Qt::NoPen);

      painter.setBrush(QColor(it->second(0), it->second(1), it->second(2)));
      QPainterPath path(QPointF(posPx - 5.0f, 29.0f));
      path.lineTo(posPx + 5.0f, 29.0f);
      path.lineTo(posPx + 5.0f, 26.0f);
      path.lineTo(posPx, 20.0f);
      path.lineTo(posPx - 5, 26.0f);
      path.closeSubpath();
      painter.drawPath(path);

      painter.setBrush(Qt::NoBrush);
      painter.setPen(palette().color(group, QPalette::Light));
      QPainterPath hilight(QPointF(posPx - 5.0f, 29.0f));
      hilight.lineTo(posPx - 5.0f, 26.0f);
      hilight.lineTo(posPx, 20.0f);
      painter.drawPath(hilight);

      painter.setPen(palette().color(group, QPalette::Shadow));
      QPainterPath shadow(QPointF(posPx, 20.0f));
      shadow.lineTo(posPx + 5.0f, 26.0f);
      shadow.lineTo(posPx + 5.0f, 29.0f);
      shadow.lineTo(posPx - 5.0f, 29.0f);
      painter.drawPath(shadow);
    }
  }

}
