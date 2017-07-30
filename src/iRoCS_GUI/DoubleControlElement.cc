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

#include "DoubleControlElement.hh"

#include <QtGui/QHBoxLayout>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QSlider>

#ifdef HAVE_BLITZ_V9
#include <blitz/tinyvec-et.h>
#endif

namespace iRoCS {

  DoubleControlElement::DoubleControlElement(
    QString const &label, double value, QWidget *parent)
    : ControlElement(label, parent), _value(value), p_spinbox(NULL),
    p_slider(NULL) {
    QWidget *controls = new QWidget;
    QHBoxLayout *layout = new QHBoxLayout(controls);
    layout->setContentsMargins(0, 0, 0, 0);
    p_spinbox = new QDoubleSpinBox;
    p_spinbox->setRange(
      -std::numeric_limits<double>::infinity(),
      std::numeric_limits<double>::infinity());
    p_spinbox->setSingleStep(0.1);
    p_spinbox->setDecimals(3);
    p_spinbox->setValue(_value);
    p_spinbox->setMinimumWidth(80);
    p_spinbox->setKeyboardTracking(false);
    layout->addWidget(p_spinbox);
    connect(p_spinbox, SIGNAL(valueChanged(double)), SLOT(_setValue(double)));
    setControlWidget(controls);
  }

  DoubleControlElement::~DoubleControlElement() {}

  void DoubleControlElement::setSliderVisible(bool enable) {
    if (controlWidget() == NULL || (p_slider == NULL && !enable)) return;

    QHBoxLayout *controlLayout = static_cast<QHBoxLayout*>(
      controlWidget()->layout());

    if (enable && static_cast<int>(
      (p_spinbox->maximum() - p_spinbox->minimum()) /
      p_spinbox->singleStep()) < 1000000) {
      p_slider = new QSlider;
      p_slider->setRange(
        0, static_cast<int>(
        (p_spinbox->maximum() - p_spinbox->minimum()) /
          p_spinbox->singleStep()));
      p_slider->setSingleStep(1);
      p_slider->setOrientation(Qt::Horizontal);
      p_slider->setSizePolicy(
        QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
      updateSlider(p_spinbox->value());
      p_spinbox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
      connect(p_slider, SIGNAL(sliderMoved(int)), SLOT(updateSpinBox(int)));
      connect(p_spinbox, SIGNAL(valueChanged(double)),
        SLOT(updateSlider(double)));
      controlLayout->insertWidget(controlLayout->count() - 1, p_slider);
    }
    else {
      disconnect(p_spinbox, SIGNAL(valueChanged(double)),
        this, SLOT(updateSlider(double)));
      controlLayout->removeWidget(p_slider);
      delete p_slider;
      p_slider = NULL;
      p_spinbox->setSizePolicy(
        QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    }
  }

  bool DoubleControlElement::sliderVisible() const {
    return p_slider != NULL;
  }

  void DoubleControlElement::setRange(double min, double max) {
    p_spinbox->setRange(min, max);
    if (p_slider != NULL && static_cast<int>(
      (p_spinbox->maximum() - p_spinbox->minimum()) /
      p_spinbox->singleStep()) < 1000000) {
      p_slider->setRange(
        0, static_cast<int>(
        (p_spinbox->maximum() - p_spinbox->minimum()) /
          p_spinbox->singleStep()));
      p_slider->setPageStep(
        (p_slider->maximum() / 100 > 1) ? p_slider->maximum() / 100 : 1);
      updateSlider(p_spinbox->value());
    }
    else {
      if (p_slider != NULL) {
        disconnect(p_spinbox, SIGNAL(valueChanged(double)),
          this, SLOT(updateSlider(double)));
        static_cast<QHBoxLayout*>(controlWidget()->layout())->removeWidget(
          p_slider);
        delete p_slider;
        p_slider = NULL;
      }
    }
  }

  void DoubleControlElement::setSingleStep(double step) {
    p_spinbox->setSingleStep(step);
    if (p_slider != NULL && static_cast<int>(
      (p_spinbox->maximum() - p_spinbox->minimum()) /
      p_spinbox->singleStep()) < 1000000) {
      p_slider->setRange(
        0, static_cast<int>(
        (p_spinbox->maximum() - p_spinbox->minimum()) /
          p_spinbox->singleStep()));
      p_slider->setPageStep(
        (p_slider->maximum() / 100 > 1) ? p_slider->maximum() / 100 : 1);
      updateSlider(p_spinbox->value());
    }
    else {
      if (p_slider != NULL) {
        disconnect(p_spinbox, SIGNAL(valueChanged(double)),
          this, SLOT(updateSlider(double)));
        static_cast<QHBoxLayout*>(controlWidget()->layout())->removeWidget(
          p_slider);
        delete p_slider;
        p_slider = NULL;
      }
    }
  }

  void DoubleControlElement::setDecimals(int decimals) {
    p_spinbox->setDecimals(decimals);
  }

  void DoubleControlElement::setSpecialValueText(QString const &text) {
    p_spinbox->setSpecialValueText(text);
  }

  void DoubleControlElement::setSuffix(QString const &text) {
    p_spinbox->setSuffix(text);
  }

  double DoubleControlElement::value() const {
    return _value;
  }

  std::string DoubleControlElement::toStdString() const {
    std::stringstream outStream;
    outStream << value();
    return outStream.str();
  }

  void DoubleControlElement::setValue(double value) {
    if (value == _value) return;
    p_spinbox->setValue(value);
  }

  void DoubleControlElement::setValue(const std::string& value) {
    double val;
    std::stringstream inStream(value);
    inStream >> val;
    setValue(val);
  }

  void DoubleControlElement::_setValue(double value) {
    if (_value == value) return;
    _value = value;
    emitValueChanged();
  }

  void DoubleControlElement::updateSpinBox(int value) {
    if (static_cast<double>(value) * p_spinbox->singleStep() +
      p_spinbox->minimum() == p_spinbox->value()) return;
    p_spinbox->setValue(
      static_cast<double>(value) * p_spinbox->singleStep() +
      p_spinbox->minimum());
  }

  void DoubleControlElement::updateSlider(double value) {
    if (static_cast<int>((value - p_spinbox->minimum()) /
      p_spinbox->singleStep()) == p_slider->value()) return;
    p_slider->setValue(
      static_cast<int>(
      (value - p_spinbox->minimum()) / p_spinbox->singleStep()));
  }

  void DoubleControlElement::emitValueChanged() {
    ControlElement::emitValueChanged();
    emit valueChanged(value());
  }

}
