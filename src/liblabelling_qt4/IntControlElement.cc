/**************************************************************************
**       Title: Widget for editing integer numbers
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
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
  p_layout->addWidget(p_spinbox);
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
