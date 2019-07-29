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

#include "StringSelectionControlElement.hh"

#include <QtGui/QHBoxLayout>
#include <QtGui/QComboBox>
#include <QtCore/QStringList>

StringSelectionControlElement::StringSelectionControlElement(
    const QString& label, QStringList const &elements, QWidget* parent)
        : AbstractControlElement(label, parent)
{
  p_combobox = new QComboBox;
  p_combobox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  p_combobox->addItems(elements);
  p_combobox->setEditable(false);
  p_controlLayout->addWidget(p_combobox);

  connect(p_combobox, SIGNAL(activated(int)), SIGNAL(activated(int)));
  connect(p_combobox, SIGNAL(activated(int)), SIGNAL(valueChanged(int)));
  connect(p_combobox, SIGNAL(activated(int)), SIGNAL(valueChanged()));
  connect(p_combobox, SIGNAL(activated(const QString&)),
          SIGNAL(activated(const QString&)));
  connect(p_combobox, SIGNAL(currentIndexChanged(int)),
          SIGNAL(currentIndexChanged(int)));
  connect(p_combobox, SIGNAL(currentIndexChanged(const QString&)),
          SIGNAL(currentIndexChanged(const QString&)));
  connect(p_combobox, SIGNAL(highlighted(int)), SIGNAL(highlighted(int)));
  connect(p_combobox, SIGNAL(highlighted(const QString&)),
          SIGNAL(highlighted(const QString&)));
}

StringSelectionControlElement::~StringSelectionControlElement()
{}

int StringSelectionControlElement::value() const
{
  return p_combobox->currentIndex();
}

int StringSelectionControlElement::nValues() const
{
  return p_combobox->count();
}

std::string StringSelectionControlElement::toStdString() const
{
  return p_combobox->currentText().toStdString();
}

void StringSelectionControlElement::setValue(const int value)
{
  if (value >= 0 && value < p_combobox->count())
      p_combobox->setCurrentIndex(value);
}

void StringSelectionControlElement::setValue(const std::string& value)
{
  int index = p_combobox->findText(value.c_str());
  setValue(index);
}
