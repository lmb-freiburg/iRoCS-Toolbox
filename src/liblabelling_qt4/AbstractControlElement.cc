#include "AbstractControlElement.hh"

#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QCheckBox>

#include <iostream>

AbstractControlElement::AbstractControlElement(
    const QString& label, QWidget* parent)
        : QWidget(parent), p_switch(NULL)
{
  p_outerLayout = new QHBoxLayout;
  p_outerLayout->setContentsMargins(0, 0, 0, 0);

  p_control = new QWidget;
  p_layout = new QHBoxLayout;
  p_layout->setContentsMargins(0, 0, 0, 0);
  p_label = new QLabel(label);
  if (label != "") p_layout->addWidget(p_label);
  p_control->setLayout(p_layout);
  p_outerLayout->addWidget(p_control);

  setLayout(p_outerLayout);
}

AbstractControlElement::~AbstractControlElement()
{}

void AbstractControlElement::setSwitchable(const bool switchable)
{
  if (switchable == this->switchable()) return;
  if (!switchable)
  {
    p_outerLayout->removeWidget(p_switch);
    delete p_switch;
    p_switch = NULL;
    p_control->setEnabled(true);
  }
  else 
  {
    p_switch = new QCheckBox;
    p_switch->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    p_switch->setVisible(true);
    p_switch->setChecked(true);
    p_outerLayout->insertWidget(0, p_switch);
    connect(p_switch, SIGNAL(toggled(bool)),
            SIGNAL(activationStateChanged(bool)));
    connect(p_switch, SIGNAL(toggled(bool)), p_control, SLOT(setEnabled(bool)));
    p_control->setEnabled(p_switch->isChecked());
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
  if (p_label->text() == "") p_layout->insertWidget(0, p_label);
  if (label == "") p_layout->removeWidget(p_label);
  p_label->setVisible(label != "");
  p_label->setText(label);
}
