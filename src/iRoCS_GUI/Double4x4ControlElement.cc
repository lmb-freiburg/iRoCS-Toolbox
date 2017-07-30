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

#include "Double4x4ControlElement.hh"

#include <QtGui/QGridLayout>
#include <QtGui/QDoubleSpinBox>

#ifdef HAVE_BLITZ_V9
namespace blitz {

  template<typename DataT, int NRows, int NCols>
  blitz::TinyMatrix<bool, NRows, NCols> operator==(
    blitz::TinyMatrix<DataT, NRows, NCols> const &lhs,
    blitz::TinyMatrix<DataT, NRows, NCols> const &rhs) {
    blitz::TinyMatrix<bool, NRows, NCols> res;
    for (int i = 0; i < NRows * NCols; ++i)
      res.dataFirst()[i] = lhs.dataFirst()[i] == rhs.dataFirst()[i];
    return res;
  }

  template<int NRows, int NCols>
  bool all(blitz::TinyMatrix<bool, NRows, NCols> const &mat) {
    for (int i = 0; i < NRows * NCols; ++i)
      if (!mat.dataFirst()[i]) return false;
    return true;
  }

}
#endif

namespace iRoCS {

  Double4x4ControlElement::Double4x4ControlElement(
    QString const &label, blitz::TinyMatrix<double, 4, 4> const &value,
    QWidget *parent)
    : ControlElement(label, parent), _value(value) {
    QWidget *controls = new QWidget;
    QGridLayout *layout = new QGridLayout(controls);
    layout->setContentsMargins(0, 0, 0, 0);
    for (int r = 0; r < 4; ++r) {
      for (int c = 0; c < 4; ++c) {
        _spinbox(r, c) = new QDoubleSpinBox;
        _spinbox(r, c)->setRange(
          -std::numeric_limits<double>::infinity(),
          std::numeric_limits<double>::infinity());
        _spinbox(r, c)->setSingleStep(0.1);
        _spinbox(r, c)->setDecimals(3);
        _spinbox(r, c)->setValue(_value(r, c));
        _spinbox(r, c)->setMinimumWidth(80);
        _spinbox(r, c)->setKeyboardTracking(false);
        layout->addWidget(_spinbox(r, c), r, c);
        connect(_spinbox(r, c), SIGNAL(valueChanged(double)),
          SLOT(setValue(double)));
      }
    }
    setControlWidget(controls);
  }

  Double4x4ControlElement::~Double4x4ControlElement() {}

  void Double4x4ControlElement::setRange(
    blitz::TinyMatrix<double, 4, 4> const &min,
    blitz::TinyMatrix<double, 4, 4> const &max) {
    for (int r = 0; r < 4; ++r)
      for (int c = 0; c < 4; ++c)
        _spinbox(r, c)->setRange(min(r, c), max(r, c));
  }

  void Double4x4ControlElement::setSingleStep(
    blitz::TinyMatrix<double, 4, 4> const &step) {
    for (int r = 0; r < 4; ++r)
      for (int c = 0; c < 4; ++c)
        _spinbox(r, c)->setSingleStep(step(r, c));
  }

  void Double4x4ControlElement::setDecimals(
    blitz::TinyMatrix<int, 4, 4> const &decimals) {
    for (int r = 0; r < 4; ++r)
      for (int c = 0; c < 4; ++c)
        _spinbox(r, c)->setDecimals(decimals(r, c));
  }

  blitz::TinyMatrix<double, 4, 4> const &Double4x4ControlElement::value() const {
    return _value;
  }

  std::string Double4x4ControlElement::toStdString() const {
    std::stringstream outStream;
    outStream << value();
    return outStream.str();
  }

  void Double4x4ControlElement::setValue(
    blitz::TinyMatrix<double, 4, 4> const &value) {
    if (blitz::all(value == _value)) return;
    _value = value;
    for (int r = 0; r < 4; ++r) {
      for (int c = 0; c < 4; ++c) {
        disconnect(_spinbox(r, c), SIGNAL(valueChanged(double)),
          this, SLOT(setValue(double)));
        _spinbox(r, c)->setValue(value(r, c));
        connect(_spinbox(r, c), SIGNAL(valueChanged(double)),
          SLOT(setValue(double)));
      }
    }
    emitValueChanged();
  }

  void Double4x4ControlElement::setValue(const std::string& value) {
    blitz::TinyMatrix<double, 4, 4> mat;
    std::stringstream inStream(value);
    inStream >> mat;
    setValue(mat);
  }

  void Double4x4ControlElement::setValue(double value) {
    if (sender() == NULL) return;
    int index = 0;
    while (index < 16 && _spinbox.dataFirst()[index] != sender()) index++;
    if (index == 16 || _value.dataFirst()[index] == value) return;
    _value.dataFirst()[index] = value;
    emitValueChanged();
  }

  void Double4x4ControlElement::emitValueChanged() {
    ControlElement::emitValueChanged();
    emit valueChanged(value());
  }

}
