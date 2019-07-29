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

#include "StringControlElement.hh"

#include <QtGui/QHBoxLayout>
#include <QtGui/QLineEdit>

StringControlElement::StringControlElement(
    const QString& label, const std::string& value, QWidget* parent)
        : AbstractControlElement(label, parent)
{
  p_lineedit = new QLineEdit;
  p_lineedit->setText(value.c_str());
  p_controlLayout->addWidget(p_lineedit);
  connect(p_lineedit, SIGNAL(editingFinished()), SLOT(emitValueChange()));
}

StringControlElement::~StringControlElement()
{}

std::string StringControlElement::value() const
{
  return p_lineedit->text().toStdString();
}

std::string StringControlElement::toStdString() const
{
  return value();
}

void StringControlElement::setValue(const std::string& value)
{
  p_lineedit->setText(value.c_str());
}

void StringControlElement::emitValueChange()
{
  emit AbstractControlElement::valueChanged();
  emit valueChanged(p_lineedit->text().toStdString());
}
