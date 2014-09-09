/**************************************************************************
**       Title: Widget for editing strings
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
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
  p_layout->addWidget(p_lineedit);
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
