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

#include "Int3ControlElement.hh"

#include <QtGui/QHBoxLayout>
#include <QtGui/QSpinBox>

Int3ControlElement::Int3ControlElement(
    const QString& label, const blitz::TinyVector<int,3>& value,
    QWidget* parent)
        : AbstractControlElement(label, parent)
{
  for (int d = 0; d < 3; ++d)
  {
    _spinbox(d) = new QSpinBox;
    _spinbox(d)->setRange(INT_MIN, INT_MAX);
    _spinbox(d)->setValue(value(d));
    _spinbox(d)->setMinimumWidth(80);
    _spinbox(d)->setKeyboardTracking(false);
    p_controlLayout->addWidget(_spinbox(d), 1);
    connect(_spinbox(d), SIGNAL(valueChanged(int)), SIGNAL(valueChanged()));
  }
}

Int3ControlElement::~Int3ControlElement()
{}

void Int3ControlElement::setRange(const blitz::TinyVector<int,3>& min,
                                  const blitz::TinyVector<int,3>& max)
{
  for (int d = 0; d < 3; ++d) _spinbox(d)->setRange(min(d), max(d));
}

blitz::TinyVector<int,3> Int3ControlElement::value() const
{
  return blitz::TinyVector<int,3>(
      _spinbox(0)->value(), _spinbox(1)->value(), _spinbox(2)->value());
}

std::string Int3ControlElement::toStdString() const
{
  std::stringstream outStream;
  outStream << value();
  return outStream.str();
}

void Int3ControlElement::setValue(const blitz::TinyVector<int,3>& value)
{
  for (int d = 0; d < 3; ++d) _spinbox(d)->setValue(value(d));
}

void Int3ControlElement::setValue(const std::string& value)
{
  blitz::TinyVector<int,3> vec;
  std::stringstream inStream(value);
  inStream >> vec;
  for (int d = 0; d < 3; ++d) _spinbox(d)->setValue(vec(d));
}
