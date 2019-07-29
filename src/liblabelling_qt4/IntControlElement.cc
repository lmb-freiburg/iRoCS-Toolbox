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

#include "IntControlElement.hh"

#include <QtGui/QHBoxLayout>
#include <QtGui/QSpinBox>

IntControlElement::IntControlElement(
    const QString& label, const int value, QWidget* parent)
        : AbstractControlElement(label, parent)
{
  p_spinbox = new QSpinBox;
  p_spinbox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  p_spinbox->setRange(INT_MIN, INT_MAX);
  p_spinbox->setValue(value);
  p_spinbox->setMinimumWidth(80);
  p_spinbox->setKeyboardTracking(false);
  p_controlLayout->addWidget(p_spinbox);
  connect(p_spinbox, SIGNAL(valueChanged(int)), SIGNAL(valueChanged()));
  connect(p_spinbox, SIGNAL(valueChanged(int)), SIGNAL(valueChanged(int)));
}

IntControlElement::~IntControlElement()
{}

void IntControlElement::setRange(const int min, const int max)
{
  p_spinbox->setRange(min, max);
}

void IntControlElement::setSpecialValueText(QString const &text)
{
  p_spinbox->setSpecialValueText(text);
}

void IntControlElement::setSuffix(QString const &text)
{
  p_spinbox->setSuffix(text);
}

int IntControlElement::value() const
{
  return p_spinbox->value();
}

std::string IntControlElement::toStdString() const
{
  return QString::number(value()).toStdString();
}

void IntControlElement::setValue(const int value)
{
  p_spinbox->setValue(value);
}

void IntControlElement::setValue(const std::string& value)
{
  p_spinbox->setValue(atoi(value.c_str()));
}
