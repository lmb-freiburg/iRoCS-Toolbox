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

#include "IntVectorControlElement.hh"

#include <QtGui/QHBoxLayout>
#include <QtGui/QSpinBox>

#include <sstream>

IntVectorControlElement::IntVectorControlElement(
    const QString& label, const std::vector<int>& value,
    QWidget* parent)
        : AbstractControlElement(label, parent), _min(INT_MIN), _max(INT_MAX)
{
  _spinbox.resize(value.size());
  for (size_t d = 0; d < value.size(); ++d)
  {
    _spinbox[d] = new QSpinBox;
    _spinbox[d]->setRange(_min, _max);
    _spinbox[d]->setValue(value[d]);
    _spinbox[d]->setMinimumWidth(80);
    _spinbox[d]->setKeyboardTracking(false);
    p_controlLayout->addWidget(_spinbox[d], 1);
    connect(_spinbox[d], SIGNAL(valueChanged(int)), SIGNAL(valueChanged()));
  }
}

IntVectorControlElement::~IntVectorControlElement()
{}

void IntVectorControlElement::setRange(const int min, const int max)
{
  _min = min;
  _max = max;
  for (size_t d = 0; d < _spinbox.size(); ++d) _spinbox[d]->setRange(min, max);
}

void IntVectorControlElement::setLength(size_t length)
{
  while (_spinbox.size() > length) pop_back();
  while (length > _spinbox.size()) push_back(-1);
}

size_t IntVectorControlElement::length() const
{
  return _spinbox.size();
}

void IntVectorControlElement::push_back(const int value)
{
  _spinbox.push_back(new QSpinBox);
  _spinbox.back()->setRange(_min, _max);
  _spinbox.back()->setValue(value);
  _spinbox.back()->setMinimumWidth(80);
  p_controlLayout->insertWidget(
      static_cast<int>(_spinbox.size()), _spinbox.back());
  connect(_spinbox.back(), SIGNAL(valueChanged(int)), SIGNAL(valueChanged()));
}

void IntVectorControlElement::pop_back()
{
  _spinbox.back()->setParent(0);
  delete _spinbox.back();
  _spinbox.pop_back();
}

std::vector<int> IntVectorControlElement::value() const
{
  std::vector<int> value(_spinbox.size());
  for (size_t d = 0; d < _spinbox.size(); ++d) value[d] = _spinbox[d]->value();
  return value;
}

std::string IntVectorControlElement::toStdString() const
{
  std::stringstream outStream;
  outStream << "( ";
  for (size_t d = 0; d < _spinbox.size() - 1; ++d)
      outStream << _spinbox[d]->value() << ", ";
  outStream << _spinbox.back()->value() << " )";
  return outStream.str();
}

void IntVectorControlElement::setValue(const std::vector<int>& value)
{
  setLength(value.size());
  for (size_t d = 0; d < value.size(); ++d) _spinbox[d]->setValue(value[d]);
}

void IntVectorControlElement::setValue(const std::string& value)
{
  std::vector<int> tmp;
  std::stringstream inStream(value);
  inStream.ignore(1);
  int val;
  while (!inStream.fail() && !inStream.eof())
  {
    inStream >> val;
    tmp.push_back(val);
    inStream.ignore(1);
  }
  setValue(tmp);
}
