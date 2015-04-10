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

#include "DoubleRangeControlElement.hh"

#include <QtGui/QHBoxLayout>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QLabel>

#include <limits>
#include <sstream>

DoubleRangeControlElement::DoubleRangeControlElement(
    const QString& label, double lbound, double ubound, QWidget* parent)
        : AbstractControlElement(label, parent)
{
  p_lbspinbox = new QDoubleSpinBox;
  p_lbspinbox->setRange(-std::numeric_limits<double>::infinity(), ubound);
  p_lbspinbox->setSingleStep(0.1);
  p_lbspinbox->setDecimals(6);
  p_lbspinbox->setValue(lbound);
  p_lbspinbox->setMinimumWidth(80);
  p_lbspinbox->setKeyboardTracking(false);
  connect(p_lbspinbox, SIGNAL(valueChanged(double)), SLOT(emitValueChange()));
  connect(p_lbspinbox, SIGNAL(valueChanged(double)),
          SLOT(setMinimumUpperBound(double)));
  p_layout->addWidget(p_lbspinbox);

  p_layout->addWidget(new QLabel(tr(" - ")));

  p_ubspinbox = new QDoubleSpinBox;
  p_ubspinbox->setRange(lbound, std::numeric_limits<double>::infinity());
  p_ubspinbox->setSingleStep(0.1);
  p_ubspinbox->setDecimals(6);
  p_ubspinbox->setValue(ubound);
  p_ubspinbox->setMinimumWidth(80);
  p_ubspinbox->setKeyboardTracking(false);
  connect(p_ubspinbox, SIGNAL(valueChanged(double)), SLOT(emitValueChange()));
  connect(p_ubspinbox, SIGNAL(valueChanged(double)),
          SLOT(setMaximumLowerBound(double)));
  p_layout->addWidget(p_ubspinbox);

  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

DoubleRangeControlElement::~DoubleRangeControlElement()
{}

void DoubleRangeControlElement::setRange(const double min, const double max)
{
  p_lbspinbox->setRange(min, p_ubspinbox->value());
  p_ubspinbox->setRange(p_lbspinbox->value(), max);
}

void DoubleRangeControlElement::setSingleStep(const double step)
{
  p_lbspinbox->setSingleStep(step);
  p_ubspinbox->setSingleStep(step);
}

double DoubleRangeControlElement::lowerBound() const 
{
  return p_lbspinbox->value();
}

double DoubleRangeControlElement::upperBound() const 
{
  return p_ubspinbox->value();
}

std::string DoubleRangeControlElement::toStdString() const 
{
  return (QString::number(lowerBound()) + tr(" - ") +
          QString::number(upperBound())).toStdString();
}

void DoubleRangeControlElement::setLowerBound(double lbound)
{
  if (lbound == p_lbspinbox->value()) return;
  p_lbspinbox->setValue(lbound);
}

void DoubleRangeControlElement::setUpperBound(double ubound)
{
  if (ubound == p_ubspinbox->value()) return;
  p_ubspinbox->setValue(ubound);
}

void DoubleRangeControlElement::setValue(double lbound, double ubound)
{
  if (lbound == p_lbspinbox->value() &&
      ubound == p_ubspinbox->value()) return;

  disconnect(p_lbspinbox, SIGNAL(valueChanged(double)),
             this, SLOT(emitValueChange()));
  disconnect(p_ubspinbox, SIGNAL(valueChanged(double)),
             this, SLOT(emitValueChange()));
  p_lbspinbox->setMaximum(ubound);
  p_ubspinbox->setMinimum(lbound);
  p_lbspinbox->setValue(lbound);
  p_ubspinbox->setValue(ubound);
  connect(p_lbspinbox, SIGNAL(valueChanged(double)), SLOT(emitValueChange()));
  connect(p_ubspinbox, SIGNAL(valueChanged(double)), SLOT(emitValueChange()));
  emitValueChange();
}

void DoubleRangeControlElement::setValue(const std::string& value)
{
  std::stringstream tmp(value);
  double lbound;
  double ubound;
  tmp >> lbound;
  tmp.ignore(3);
  tmp >> ubound;
  setValue(lbound, ubound);
}

void DoubleRangeControlElement::setMinimumUpperBound(double minUB)
{
  p_ubspinbox->setMinimum(minUB);
}

void DoubleRangeControlElement::setMaximumLowerBound(double maxLB)
{
  p_lbspinbox->setMaximum(maxLB);
}

void DoubleRangeControlElement::emitValueChange()
{
  emit valueChanged(p_lbspinbox->value(), p_ubspinbox->value());
  emit AbstractControlElement::valueChanged();
}
