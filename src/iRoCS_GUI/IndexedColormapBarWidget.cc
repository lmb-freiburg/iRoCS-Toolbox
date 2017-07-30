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

#include "IndexedColormapBarWidget.hh"
#include "IndexedColormap.hh"
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

namespace iRoCS
{

  /*-----------------------------------------------------------------------
   *  IndexedColormapBarWidget
   *-----------------------------------------------------------------------*/ 
  IndexedColormapBarWidget::IndexedColormapBarWidget(
      IndexedColormap &colormap, QWidget *parent)
          : QWidget(parent), IndexedColormapObserver(colormap),
            _draggedIntermediateColor(-1.0), _draggedOverrideColor(INT_MIN)
  {
    resize(sizeHint());
    setFixedHeight(sizeHint().height());
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  }
  
  IndexedColormapBarWidget::~IndexedColormapBarWidget()
  {}
  
  QSize IndexedColormapBarWidget::sizeHint() const
  {
    return QSize(256, 30);
  }

  void IndexedColormapBarWidget::updateIndexRange()
  {
    update();
  }
  
  void IndexedColormapBarWidget::updateStartColor()
  {
    update();
  }
  
  void IndexedColormapBarWidget::updateEndColor()
  {
    update();
  }
  
  void IndexedColormapBarWidget::updateIntermediateColor(double)
  {
    update();
  }
  
  void IndexedColormapBarWidget::updateMoveIntermediateColor(double, double)
  {
    update();
  }
  
  void IndexedColormapBarWidget::updateRemoveIntermediateColor(double)
  {
    update();
  }
  
  void IndexedColormapBarWidget::updateClearIntermediateColors()
  {
    update();
  }
  
  void IndexedColormapBarWidget::updateOverrideColor(int)
  {
    update();
  }
  
  void IndexedColormapBarWidget::updateMoveOverrideColor(int, int)
  {
    update();
  }
  
  void IndexedColormapBarWidget::updateRemoveOverrideColor(int)
  {
    update();
  }
  
  void IndexedColormapBarWidget::updateClearOverrideColors()
  {
    update();
  }
  
  void IndexedColormapBarWidget::updateMonochromeColor()
  {
    update();
  }
  
  void IndexedColormapBarWidget::updateRangeIndicator()
  {
    update();
  }
  
  void IndexedColormapBarWidget::updateGamma()
  {
    update();
  }
  
  void IndexedColormapBarWidget::updateColormap()
  {
    update();
  }

  void IndexedColormapBarWidget::resizeEvent(QResizeEvent *)
  {
    update();
  }

  void IndexedColormapBarWidget::mousePressEvent(QMouseEvent *e)
  {
    e->ignore();
    if (e->button() == Qt::LeftButton)
    {
      if (e->pos().y() >= 15) // Possible intermediate color drag initiation
      {
        if (_colormap.intermediateColors().size() > 0)
        {
          std::map<double,IndexedColormap::ColorT>::const_iterator it =
              _colormap.intermediateColors().begin();
          float posPx =
              std::pow(it->first, _colormap.gamma()) * (width() - 1);
          float minDist = std::abs(posPx - static_cast<float>(e->pos().x()));
          double draggedPos = it->first;
          ++it;
          for (; it != _colormap.intermediateColors().end(); ++it)
          {
            float posPx =
                std::pow(it->first, _colormap.gamma()) * (width() - 1);
            float dist = std::abs(posPx - static_cast<float>(e->pos().x()));
            if (dist < minDist)
            {
              minDist = dist;
              draggedPos = it->first;
            }
          }
          if (minDist < 5)
          {
            _draggedIntermediateColor = draggedPos;
            e->accept();
            return;
          }
        }
      }
      if (e->pos().y() <= 10) // Possible override color drag initiation
      {
        if (_colormap.overrideColors().size() > 0)
        {
          std::map<int,IndexedColormap::ColorT>::const_iterator it =
              _colormap.overrideColors().begin();
          double scale = _colormap.endIndex() - _colormap.startIndex() + 1;
          double offset = _colormap.startIndex() - 0.5;
          float posPx = (it->first - offset) / scale * (width() - 1);
          float minDist = std::abs(posPx - static_cast<float>(e->pos().x()));
          int draggedIndex = it->first;
          for (; it != _colormap.overrideColors().end(); ++it)
          {
            float posPx = (it->first - offset) / scale * (width() - 1);
            float dist = std::abs(posPx - static_cast<float>(e->pos().x()));
            if (dist < minDist)
            {
              minDist = dist;
              draggedIndex = it->first;
            }
          }
          if (minDist < 5)
          {
            _draggedOverrideColor = draggedIndex;
            e->accept();
            return;
          }
        }
      }
    }
    if (e->button() == Qt::RightButton)
    {
      if (e->pos().y() >= 15) // Possible intermediate color context request
      {
        if (_colormap.intermediateColors().size() > 0)
        {
          std::map<double,IndexedColormap::ColorT>::const_iterator it =
              _colormap.intermediateColors().begin();
          float posPx =
              std::pow(it->first, _colormap.gamma()) * (width() - 1);
          float minDist = std::abs(posPx - static_cast<float>(e->pos().x()));
          double selectedPos = it->first;
          ++it;
          for (; it != _colormap.intermediateColors().end(); ++it)
          {
            float posPx =
                std::pow(it->first, _colormap.gamma()) * (width() - 1);
            float dist = std::abs(posPx - static_cast<float>(e->pos().x()));
            if (dist < minDist)
            {
              minDist = dist;
              selectedPos = it->first;
            }
          }
          if (minDist < 5)
          {
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
            IndexedColormap::ColorT oldColor =
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
            else
            {
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
      if (e->pos().y() <= 10) // Possible override color context request
      {
        if (_colormap.overrideColors().size() > 0)
        {
          std::map<int,IndexedColormap::ColorT>::const_iterator it =
              _colormap.overrideColors().begin();
          double scale = _colormap.endIndex() - _colormap.startIndex() + 1;
          double offset = _colormap.startIndex() - 0.5;
          float posPx = (it->first - offset) / scale * (width() - 1);
          float minDist = std::abs(posPx - static_cast<float>(e->pos().x()));
          int selectedIndex = it->first;
          for (; it != _colormap.overrideColors().end(); ++it)
          {
            float posPx = (it->first - offset) / scale * (width() - 1);
            float dist = std::abs(posPx - static_cast<float>(e->pos().x()));
            if (dist < minDist)
            {
              minDist = dist;
              selectedIndex = it->first;
            }
          }
          if (minDist < 5)
          {
            QDialog overrideColorEditor(this, Qt::Popup);
            QHBoxLayout *layout = new QHBoxLayout(&overrideColorEditor);
            layout->setContentsMargins(0, 0, 0, 0);
            QFrame *frame = new QFrame;
            frame->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
            QHBoxLayout *frameLayout = new QHBoxLayout(frame);
            frameLayout->setContentsMargins(0, 0, 0, 0);
            QToolButton *removeButton = new QToolButton;
            removeButton->setIcon(QIcon::fromTheme("edit-delete"));
            frameLayout->addWidget(removeButton);
            connect(removeButton, SIGNAL(clicked()),
                    &overrideColorEditor, SLOT(accept()));
            QLabel *posLabel = new QLabel(tr("Pos:"));
            frameLayout->addWidget(posLabel);
            QSpinBox *positionSpinBox = new QSpinBox;
            positionSpinBox->setMinimum(INT_MIN);
            positionSpinBox->setMaximum(INT_MAX);
            positionSpinBox->setValue(selectedIndex);
            frameLayout->addWidget(positionSpinBox);
            ColorControlElement *colorControl =
                new ColorControlElement(tr("Color:"));
            IndexedColormap::ColorT oldColor =
                _colormap.overrideColors().find(selectedIndex)->second;
            colorControl->setValue(oldColor);
            frameLayout->addWidget(colorControl);
            layout->addWidget(frame);

            // This is a workaround to ensure that the geometry calculation
            // is done
            overrideColorEditor.move(-5000, -5000);
            overrideColorEditor.show();
            
            // Ensure that the whole editor widget is visible on the current
            // screen
            QPoint editorPosition(QCursor::pos());
            QRect screenGeometry(
                QApplication::desktop()->screenGeometry(
                    QApplication::desktop()->screenNumber(this)));
            if (editorPosition.x() - screenGeometry.x() +
                overrideColorEditor.width() > screenGeometry.width())
                editorPosition.rx() = screenGeometry.x() +
                    screenGeometry.width() - overrideColorEditor.width() - 1;
            if (editorPosition.y() - screenGeometry.y() +
                overrideColorEditor.height() > screenGeometry.height())
                editorPosition.ry() = screenGeometry.y() +
                    screenGeometry.height() -
                    overrideColorEditor.height() - 1;            
            overrideColorEditor.move(editorPosition.x(), editorPosition.y());

            int res = overrideColorEditor.exec();
            if (res == QDialog::Accepted)
                _colormap.removeOverrideColor(selectedIndex);
            else
            {
              if (blitz::any(oldColor != colorControl->value()))
                  _colormap.setOverrideColor(
                      selectedIndex, colorControl->value());
              if (positionSpinBox->value() != selectedIndex)
                  _colormap.moveOverrideColor(
                      selectedIndex, positionSpinBox->value());
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
      QAction *addOverrideColorAction =
          new QAction(tr("Add override color"), &menu);
      menu.addAction(addOverrideColorAction);
      QAction *clearIntermediateColorsAction =
          new QAction(tr("Clear intermediate colors"), &menu);
      menu.addAction(clearIntermediateColorsAction);
      QAction *clearOverrideColorsAction =
          new QAction(tr("Clear override colors"), &menu);
      menu.addAction(clearOverrideColorsAction);
      QAction *res = menu.exec(QCursor::pos());
      if (res == addIntermediateColorAction)
      {
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
      else if (res == addOverrideColorAction)
      {
        double scale = _colormap.endIndex() - _colormap.startIndex() + 1;
        double offset = _colormap.startIndex();
        int pos = static_cast<int>(
            std::floor(static_cast<double>(e->pos().x()) /
                       static_cast<double>(width() - 1) * scale + offset));
        _colormap.setOverrideColor(pos, IndexedColormap::ColorT(0, 0, 0));
      }
      else if (res == clearIntermediateColorsAction)
          _colormap.clearIntermediateColors();
      else if (res == clearOverrideColorsAction)
          _colormap.clearOverrideColors();
      e->accept();
      return;
    }
  }
  
  void IndexedColormapBarWidget::mouseReleaseEvent(QMouseEvent *e)
  {
    e->ignore();
    _draggedIntermediateColor = -1.0;
    _draggedOverrideColor = INT_MIN;
    e->accept();
  }
  
  void IndexedColormapBarWidget::mouseMoveEvent(QMouseEvent *e)
  {
    e->ignore();
    if (_draggedIntermediateColor != -1.0)
    {
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
    else if (_draggedOverrideColor != INT_MIN)
    {
      int oldPos = _draggedOverrideColor;
      double scale = _colormap.endIndex() - _colormap.startIndex() + 1;
      double offset = _colormap.startIndex();
      int newPos = static_cast<int>(
          std::floor(static_cast<double>(e->pos().x()) /
                     static_cast<double>(width() - 1) * scale + offset));
      _draggedOverrideColor =
          _colormap.moveOverrideColor(oldPos, newPos);
      if (_draggedIntermediateColor != oldPos) update();
      e->accept();
      return;
    }
  }
  
  void IndexedColormapBarWidget::mouseDoubleClickEvent(QMouseEvent *e)
  {
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
  
  void IndexedColormapBarWidget::paintEvent(QPaintEvent*)
  {
    QImage colorbar(QSize(width(), 1), QImage::Format_RGB32);
    for (int x = 0; x < width(); ++x)
    {
      IndexedColormap::ColorT color = _colormap.color(
          static_cast<int>(
              std::floor(
                  static_cast<double>(x) / static_cast<double>(width() - 1) *
                  (_colormap.endIndex() - _colormap.startIndex() + 1) +
                  _colormap.startIndex()) + 0.5));
      colorbar.setPixel(x, 0, QColor(color(0), color(1), color(2)).rgb());
    }
    QPainter painter(this);
    painter.drawImage(
        QRect(0, 5, width(), 20), colorbar, QRect(0, 0, width(), 1));
    
    QPalette::ColorGroup group =
        (isEnabled()) ? QPalette::Active : QPalette::Inactive;

    for (std::map<double,IndexedColormap::ColorT>::const_iterator it =
             _colormap.intermediateColors().begin();
         it != _colormap.intermediateColors().end(); ++it)
    {
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

    for (std::map<int,IndexedColormap::ColorT>::const_iterator it =
             _colormap.overrideColors().begin();
         it != _colormap.overrideColors().end(); ++it)
    {
      double scale = _colormap.endIndex() - _colormap.startIndex() + 1;
      double offset = _colormap.startIndex() - 0.5;
      float posPx = (it->first - offset) / scale * (width() - 1);

      painter.setBrush(QColor(it->second(0), it->second(1), it->second(2)));
      painter.setPen(Qt::NoPen);
      QPainterPath path(QPointF(posPx - 5.0f, 1.0f));
      path.lineTo(posPx + 5.0f, 1.0f);
      path.lineTo(posPx + 5.0f, 4.0f);
      path.lineTo(posPx, 10.0f);
      path.lineTo(posPx - 5, 4.0f);
      path.closeSubpath();
      painter.drawPath(path);

      painter.setBrush(Qt::NoBrush);
      painter.setPen(palette().color(group, QPalette::Light));
      QPainterPath hilight(QPointF(posPx, 10.0f));
      hilight.lineTo(posPx - 5.0, 4.0f);
      hilight.lineTo(posPx - 5.0, 1.0f);
      hilight.lineTo(posPx + 5.0, 1.0f);
      painter.drawPath(hilight);

      painter.setPen(palette().color(group, QPalette::Shadow));
      QPainterPath shadow(QPointF(posPx, 10.0f));
      shadow.lineTo(posPx + 5.0f, 4.0f);
      shadow.lineTo(posPx + 5.0f, 1.0f);
      painter.drawPath(shadow);
    }
  }
  
}
