/**************************************************************************
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

#include "ControlElement.hh"

#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QCheckBox>

#include <iostream>

namespace iRoCS {

  ControlElement::ControlElement(QString const &label, QWidget *parent)
    : QWidget(parent), _switchable(false), p_controlWidget(NULL),
    p_layout(NULL), p_switchableLabelWidget(NULL),
    p_switchableLabelLayout(NULL), p_switch(NULL), p_label(NULL) {
    p_layout = new QHBoxLayout(this);
    p_layout->setContentsMargins(0, 0, 0, 0);
    p_switchableLabelWidget = new QWidget;
    p_switchableLabelLayout = new QHBoxLayout(p_switchableLabelWidget);
    p_switchableLabelLayout->setContentsMargins(0, 0, 0, 0);
    p_layout->addWidget(p_switchableLabelWidget);
    if (label != "") {
      p_label = new QLabel(label);
      p_switchableLabelLayout->addWidget(p_label);
    }
    p_switch = new QCheckBox;
    p_switch->setChecked(true);
    connect(p_switch, SIGNAL(toggled(bool)),
      SIGNAL(activationStateChanged(bool)));
  }

  ControlElement::~ControlElement() {
    // These sub-widgets can be reparented to use them in QFormLayout
    // therefore we have to check whether they have a parent and explicitly
    // delete them if not
    if (p_switchableLabelWidget->parent() == NULL)
      delete p_switchableLabelWidget;
    if (p_controlWidget->parent() == NULL) delete p_controlWidget;
  }

  void ControlElement::setSwitchable(bool switchable) {
    if (switchable == _switchable) return;
    if (!switchable) p_switchableLabelLayout->removeWidget(p_switch);
    else p_switchableLabelLayout->insertWidget(0, p_switch);
    if (p_controlWidget != NULL)
      connect(p_switch, SIGNAL(toggled(bool)),
        p_controlWidget, SLOT(setEnabled(bool)));
  }

  bool ControlElement::switchable() const {
    return _switchable;
  }

  void ControlElement::setActive(bool active) {
    if (isActive() == active) return;
    p_switch->setChecked(active);
  }

  bool ControlElement::isActive() const {
    return p_switch->isChecked();
  }

  void ControlElement::setLabelString(const QString& label) {
    if (p_label == NULL && label == "") return;
    if (p_label != NULL && p_label->text() == label) return;
    if (p_label == NULL && label != "") {
      p_label = new QLabel(label);
      p_switchableLabelLayout->insertWidget(switchable() ? 1 : 0, p_label);
      return;
    }
    if (p_label != NULL && label == "") {
      p_switchableLabelLayout->removeWidget(p_label);
      delete p_label;
      p_label = NULL;
      return;
    }
    p_label->setText(label);
  }

  QString ControlElement::labelString() const {
    return (p_label != NULL) ? p_label->text() : "";
  }

  QWidget *ControlElement::label() const {
    return p_switchableLabelWidget;
  }

  QWidget *ControlElement::controlWidget() const {
    return p_controlWidget;
  }

  void ControlElement::emitValueChanged() {
    emit valueChanged();
  }

  void ControlElement::setControlWidget(QWidget *widget) {
    if (p_controlWidget == widget) return;
    if (p_controlWidget != NULL) {
      p_layout->removeWidget(p_controlWidget);
      disconnect(p_switch, SIGNAL(toggled(bool)),
        p_controlWidget, SLOT(setEnabled(bool)));
    }
    p_controlWidget = widget;
    if (p_controlWidget != NULL) {
      p_layout->addWidget(p_controlWidget);
      connect(p_switch, SIGNAL(toggled(bool)),
        p_controlWidget, SLOT(setEnabled(bool)));
      p_controlWidget->setEnabled(isActive());
    }
  }

}
