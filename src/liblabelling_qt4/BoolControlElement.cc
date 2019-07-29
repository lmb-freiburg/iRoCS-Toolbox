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

#include "BoolControlElement.hh"

#include <QtGui/QHBoxLayout>
#include <QtGui/QCheckBox>

BoolControlElement::BoolControlElement(
    const QString& label, const bool value, QWidget* parent)
        : AbstractControlElement(label, parent)
{
  p_checkbox = new QCheckBox;
  p_checkbox->setChecked(value);
  p_controlLayout->addWidget(p_checkbox);
  connect(p_checkbox, SIGNAL(toggled(bool)), SIGNAL(valueChanged()));
  connect(p_checkbox, SIGNAL(toggled(bool)), SIGNAL(valueChanged(bool)));
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

BoolControlElement::~BoolControlElement()
{}

bool BoolControlElement::value() const
{
  return p_checkbox->isChecked();
}

std::string BoolControlElement::toStdString() const
{
  return (value()) ? std::string("true") : std::string("false");
}

void BoolControlElement::setValue(const bool value)
{
  p_checkbox->setChecked(value);
}

void BoolControlElement::setValue(const std::string& value)
{
  p_checkbox->setChecked(value == std::string("true") ||
                         value == std::string("1"));
}
