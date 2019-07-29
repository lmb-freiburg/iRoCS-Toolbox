/**************************************************************************
 *
 * Abstract horizontal Qt4 control element with label and checkbox
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

#include "AbstractControlElement.hh"

#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QCheckBox>

#include <iostream>

AbstractControlElement::AbstractControlElement(
    const QString& label, QWidget* parent)
        : QWidget(parent), p_labelLayout{new QHBoxLayout},
          p_controlLayout{new QHBoxLayout}, p_switch{NULL},
          p_label{new QLabel(label)}, p_labelWidget{new QWidget},
          p_controlWidget{new QWidget}
{
  p_labelLayout->setContentsMargins(0, 0, 0, 0);
  p_labelWidget->setLayout(p_labelLayout);
  p_controlLayout->setContentsMargins(0, 0, 0, 0);
  p_controlWidget->setLayout(p_controlLayout);

  if (label != "") p_labelLayout->addWidget(p_label);

  QHBoxLayout *layout { new QHBoxLayout };
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(p_labelWidget);
  layout->addWidget(p_controlWidget);
  setLayout(layout);
}

AbstractControlElement::~AbstractControlElement()
{}

void AbstractControlElement::setSwitchable(const bool switchable)
{
  if (switchable == this->switchable()) return;
  if (!switchable)
  {
    p_labelLayout->removeWidget(p_switch);
    delete p_switch;
    p_switch = NULL;
    p_controlWidget->setEnabled(true);
  }
  else
  {
    p_switch = new QCheckBox;
    p_switch->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    p_switch->setVisible(true);
    p_switch->setChecked(true);
    p_labelLayout->insertWidget(0, p_switch);
    connect(p_switch, SIGNAL(toggled(bool)),
            SIGNAL(activationStateChanged(bool)));
    connect(p_switch, SIGNAL(toggled(bool)),
            p_controlWidget, SLOT(setEnabled(bool)));
    p_controlWidget->setEnabled(p_switch->isChecked());
  }
}

bool AbstractControlElement::switchable() const
{
  return (p_switch != NULL);
}

void AbstractControlElement::setActive(const bool active)
{
  if (!switchable() || isActive() == active) return;
  p_switch->setChecked(active);
}

bool AbstractControlElement::isActive() const
{
  return !switchable() || p_switch->isChecked();
}

QString AbstractControlElement::label() const
{
  return p_label->text();
}

void AbstractControlElement::setLabel(const QString& label)
{
  if (label == p_label->text()) return;
  if (p_label->text() == "") p_labelLayout->addWidget(p_label);
  if (label == "") p_labelLayout->removeWidget(p_label);
  p_label->setVisible(label != "");
  p_label->setText(label);
}

QWidget *AbstractControlElement::labelWidget()
{
  return p_labelWidget;
}

QWidget *AbstractControlElement::controlWidget()
{
  return p_controlWidget;
}
