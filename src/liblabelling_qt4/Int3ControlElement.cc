/**************************************************************************
**       Title: Widget for editing 3-D vectorial integer numbers
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**************************************************************************/

#include "Int3ControlElement.hh"

#include <QtGui/QHBoxLayout>
#include <QtGui/QSpinBox>

Int3ControlElement::Int3ControlElement(
    const QString& label, const blitz::TinyVector<int,3>& value,
    QWidget* parent)
        : AbstractControlElement(label, parent)
{
  for (int d = 0; d < 3; ++d)
  {
    _spinbox(d) = new QSpinBox;
    _spinbox(d)->setRange(INT_MIN, INT_MAX);
    _spinbox(d)->setValue(value(d));
    _spinbox(d)->setMinimumWidth(80);
    _spinbox(d)->setKeyboardTracking(false);
    p_layout->addWidget(_spinbox(d), 1);
    connect(_spinbox(d), SIGNAL(valueChanged(int)), SIGNAL(valueChanged()));
  }
}

Int3ControlElement::~Int3ControlElement()
{}

void Int3ControlElement::setRange(const blitz::TinyVector<int,3>& min,
                                  const blitz::TinyVector<int,3>& max)
{
  for (int d = 0; d < 3; ++d) _spinbox(d)->setRange(min(d), max(d));
}

blitz::TinyVector<int,3> Int3ControlElement::value() const
{
  return blitz::TinyVector<int,3>(
      _spinbox(0)->value(), _spinbox(1)->value(), _spinbox(2)->value());
}

std::string Int3ControlElement::toStdString() const 
{
  std::stringstream outStream;
  outStream << value();
  return outStream.str();
}

void Int3ControlElement::setValue(const blitz::TinyVector<int,3>& value)
{
  for (int d = 0; d < 3; ++d) _spinbox(d)->setValue(value(d));
}

void Int3ControlElement::setValue(const std::string& value)
{
  blitz::TinyVector<int,3> vec;
  std::stringstream inStream(value);
  inStream >> vec;
  for (int d = 0; d < 3; ++d) _spinbox(d)->setValue(vec(d));
}
