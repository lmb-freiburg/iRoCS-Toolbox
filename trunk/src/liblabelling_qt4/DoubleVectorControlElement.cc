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

#include "DoubleVectorControlElement.hh"

#include <QtGui/QHBoxLayout>
#include <QtGui/QDoubleSpinBox>

#include <sstream>
#include <limits>

DoubleVectorControlElement::DoubleVectorControlElement(
    const QString& label, const std::vector<double>& value,
    QWidget* parent)
        : AbstractControlElement(label, parent),
          _min(-std::numeric_limits<double>::infinity()),
          _max(std::numeric_limits<double>::infinity()),
          _step(0.1)
{
  _spinbox.resize(value.size());
  for (size_t d = 0; d < value.size(); ++d)
  {
    _spinbox[d] = new QDoubleSpinBox;
    _spinbox[d]->setRange(_min, _max);
    _spinbox[d]->setSingleStep(_step);
    _spinbox[d]->setDecimals(5);
    _spinbox[d]->setValue(value[d]);
    _spinbox[d]->setMinimumWidth(80);
    _spinbox[d]->setKeyboardTracking(false);
    p_layout->addWidget(_spinbox[d], 1);
    connect(_spinbox[d], SIGNAL(valueChanged(double)), SIGNAL(valueChanged()));
  }
}

DoubleVectorControlElement::~DoubleVectorControlElement()
{}

void DoubleVectorControlElement::setRange(const double min, const double max)
{
  _min = min;
  _max = max;
  for (size_t d = 0; d < _spinbox.size(); ++d) _spinbox[d]->setRange(min, max);
}

void DoubleVectorControlElement::setSingleStep(const double step)
{
  _step = step;
  for (size_t d = 0; d < _spinbox.size(); ++d) _spinbox[d]->setSingleStep(step);
}

void DoubleVectorControlElement::setLength(size_t length)
{
  while (_spinbox.size() > length) pop_back();
  while (length > _spinbox.size()) push_back(-1.0);
}

size_t DoubleVectorControlElement::length() const
{
  return _spinbox.size();
}

void DoubleVectorControlElement::push_back(const double value)
{
  _spinbox.push_back(new QDoubleSpinBox);
  _spinbox.back()->setRange(_min, _max);
  _spinbox.back()->setSingleStep(_step);
  _spinbox.back()->setDecimals(5);
  _spinbox.back()->setValue(value);
  _spinbox.back()->setMinimumWidth(80);
  p_layout->insertWidget(static_cast<int>(_spinbox.size()), _spinbox.back());
  connect(_spinbox.back(), SIGNAL(valueChanged(double)),
          SIGNAL(valueChanged()));
}

void DoubleVectorControlElement::pop_back()
{
  _spinbox.back()->setParent(0);
  delete _spinbox.back();
  _spinbox.pop_back();
}

std::vector<double> DoubleVectorControlElement::value() const
{
  std::vector<double> value(_spinbox.size());
  for (size_t d = 0; d < _spinbox.size(); ++d) value[d] = _spinbox[d]->value();
  return value;
}

std::string DoubleVectorControlElement::toStdString() const 
{
  std::stringstream outStream;
  outStream << "( ";
  for (size_t d = 0; d < _spinbox.size() - 1; ++d)
      outStream << _spinbox[d]->value() << ", ";
  outStream << _spinbox.back()->value() << " )";
  return outStream.str();
}

void DoubleVectorControlElement::setValue(const std::vector<double>& value)
{
  setLength(value.size());
  for (size_t d = 0; d < value.size(); ++d) _spinbox[d]->setValue(value[d]);
}

void DoubleVectorControlElement::setValue(const std::string& value)
{
  std::vector<double> tmp;
  std::stringstream inStream(value);
  inStream.ignore(1);
  double val;
  while (!inStream.fail() && !inStream.eof())
  {
    inStream >> val;
    tmp.push_back(val);
    inStream.ignore(1);
  }
  setValue(tmp);
}
