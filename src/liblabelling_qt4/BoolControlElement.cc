/**************************************************************************
**       Title: Widget for editing booleans
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
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
  p_layout->addWidget(p_checkbox);
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
