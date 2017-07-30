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

#include "Double3ControlElement.hh"

#include <QtGui/QHBoxLayout>
#include <QtGui/QDoubleSpinBox>

#ifdef HAVE_BLITZ_V9
#include <blitz/tinyvec-et.h>
#endif

namespace iRoCS {

  Double3ControlElement::Double3ControlElement(
    QString const &label, blitz::TinyVector<double, 3> const &value,
    QWidget *parent)
    : ControlElement(label, parent), _value(value) {
    QWidget *controls = new QWidget;
    QHBoxLayout *layout = new QHBoxLayout(controls);
    layout->setContentsMargins(0, 0, 0, 0);
    for (int d = 0; d < 3; ++d) {
      _spinbox(d) = new QDoubleSpinBox;
      _spinbox(d)->setRange(
        -std::numeric_limits<double>::infinity(),
        std::numeric_limits<double>::infinity());
      _spinbox(d)->setSingleStep(0.1);
      _spinbox(d)->setDecimals(3);
      _spinbox(d)->setValue(_value(d));
      _spinbox(d)->setMinimumWidth(80);
      _spinbox(d)->setKeyboardTracking(false);
      layout->addWidget(_spinbox(d));
      connect(
        _spinbox(d), SIGNAL(valueChanged(double)), SLOT(setValue(double)));
    }
    setControlWidget(controls);
  }

  Double3ControlElement::~Double3ControlElement() {}

  void Double3ControlElement::setRange(
    blitz::TinyVector<double, 3> const &min,
    blitz::TinyVector<double, 3> const &max) {
    for (int d = 0; d < 3; ++d) _spinbox(d)->setRange(min(d), max(d));
  }

  void Double3ControlElement::setSingleStep(
    blitz::TinyVector<double, 3> const &step) {
    for (int d = 0; d < 3; ++d) _spinbox(d)->setSingleStep(step(d));
  }

  void Double3ControlElement::setDecimals(
    blitz::TinyVector<int, 3> const &decimals) {
    for (int d = 0; d < 3; ++d) _spinbox(d)->setDecimals(decimals(d));
  }

  blitz::TinyVector<double, 3> const &Double3ControlElement::value() const {
    return _value;
  }

  std::string Double3ControlElement::toStdString() const {
    std::stringstream outStream;
    outStream << value();
    return outStream.str();
  }

  void Double3ControlElement::setValue(const blitz::TinyVector<double, 3>& value) {
    if (blitz::all(value == _value)) return;
    _value = value;
    for (int d = 0; d < 3; ++d) {
      disconnect(_spinbox(d), SIGNAL(valueChanged(double)),
        this, SLOT(setValue(double)));
      _spinbox(d)->setValue(value(d));
      connect(_spinbox(d), SIGNAL(valueChanged(double)),
        SLOT(setValue(double)));
    }
    emitValueChanged();
  }

  void Double3ControlElement::setValue(const std::string& value) {
    blitz::TinyVector<double, 3> vec;
    std::stringstream inStream(value);
    inStream >> vec;
    setValue(vec);
  }

  void Double3ControlElement::setValue(double value) {
    if (sender() == NULL) return;
    int index = 0;
    while (index < 3 && _spinbox(index) != sender()) index++;
    if (index == 3 || _value(index) == value) return;
    _value(index) = value;
    emitValueChanged();
  }

  void Double3ControlElement::emitValueChanged() {
    ControlElement::emitValueChanged();
    emit valueChanged(value());
  }

}
