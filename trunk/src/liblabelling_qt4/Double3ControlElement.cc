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

#include "Double3ControlElement.hh"

#include <QtGui/QHBoxLayout>
#include <QtGui/QDoubleSpinBox>

Double3ControlElement::Double3ControlElement(
    const QString& label, const blitz::TinyVector<double,3>& value,
    QWidget* parent)
        : AbstractControlElement(label, parent)
{
  for (int d = 0; d < 3; ++d)
  {
    _spinbox(d) = new QDoubleSpinBox;
    _spinbox(d)->setRange(
        -std::numeric_limits<double>::infinity(), 
        std::numeric_limits<double>::infinity());
    _spinbox(d)->setSingleStep(0.1);
    _spinbox(d)->setDecimals(5);
    _spinbox(d)->setValue(value(d));
    _spinbox(d)->setMinimumWidth(80);
    _spinbox(d)->setKeyboardTracking(false);
    p_layout->addWidget(_spinbox(d), 1);
    connect(_spinbox(d), SIGNAL(valueChanged(double)), SIGNAL(valueChanged()));
  }
}

Double3ControlElement::~Double3ControlElement()
{}

void Double3ControlElement::setRange(const blitz::TinyVector<double,3>& min,
                                     const blitz::TinyVector<double,3>& max)
{
  for (int d = 0; d < 3; ++d) _spinbox(d)->setRange(min(d), max(d));
}

void Double3ControlElement::setSingleStep(
    const blitz::TinyVector<double,3>& step)
{
  for (int d = 0; d < 3; ++d) _spinbox(d)->setSingleStep(step(d));
}

blitz::TinyVector<double,3> Double3ControlElement::value() const
{
  return blitz::TinyVector<double,3>(
      _spinbox(0)->value(), _spinbox(1)->value(), _spinbox(2)->value());
}

std::string Double3ControlElement::toStdString() const 
{
  std::stringstream outStream;
  outStream << value();
  return outStream.str();
}

void Double3ControlElement::setValue(const blitz::TinyVector<double,3>& value)
{
  bool actualValueChange = false;
  for (int d = 0; d < 3; ++d)
  {
    if (_spinbox(d)->value() != value(d))
    {
      actualValueChange = true;
      disconnect(_spinbox(d), SIGNAL(valueChanged(double)),
                 this, SIGNAL(valueChanged()));
      _spinbox(d)->setValue(value(d));
      connect(_spinbox(d), SIGNAL(valueChanged(double)),
              SIGNAL(valueChanged()));
    }
  }
  if (actualValueChange) emit valueChanged();
}

void Double3ControlElement::setValue(const std::string& value)
{
  blitz::TinyVector<double,3> vec;
  std::stringstream inStream(value);
  inStream >> vec;
  setValue(vec);
}
