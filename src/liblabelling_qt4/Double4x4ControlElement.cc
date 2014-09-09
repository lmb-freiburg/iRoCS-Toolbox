/**************************************************************************
**       Title: Widget for editing 4x4 double precision matrices
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**************************************************************************/

#include "Double4x4ControlElement.hh"

#include <QtGui/QGridLayout>
#include <QtGui/QDoubleSpinBox>

#include <libArrayToolbox/ATBLinAlg.hh>

Double4x4ControlElement::Double4x4ControlElement(
    QString const &label, blitz::TinyMatrix<double,4,4> const &value,
    QWidget *parent)
        : AbstractControlElement(label, parent),
          _lastValidMatrix(atb::traits< blitz::TinyMatrix<double,4,4> >::one)
{
  QGridLayout *matrixLayout = new QGridLayout;
  for (int r = 0; r < 4; ++r)
  {
    for (int c = 0; c < 4; ++c)
    {
      _spinbox(r, c) = new QDoubleSpinBox;
      _spinbox(r, c)->setRange(
          -std::numeric_limits<double>::infinity(), 
          std::numeric_limits<double>::infinity());
      _spinbox(r, c)->setSingleStep(0.1);
      _spinbox(r, c)->setDecimals(5);
      _spinbox(r, c)->setValue(value(r, c));
      _spinbox(r, c)->setMinimumWidth(80);
      _spinbox(r, c)->setKeyboardTracking(false);
      matrixLayout->addWidget(_spinbox(r, c), r, c);
      connect(_spinbox(r, c), SIGNAL(valueChanged(double)),
              SLOT(fixMatrixAndEmitValueChanged()));
    }
  }
  try
  {
    atb::invert(value);
    _lastValidMatrix = value;
  }
  catch (...)
  {
    setValue(_lastValidMatrix);
  }
  p_layout->addLayout(matrixLayout);
}

Double4x4ControlElement::~Double4x4ControlElement()
{}

void Double4x4ControlElement::setRange(
    blitz::TinyMatrix<double,4,4> const &minVal,
    blitz::TinyMatrix<double,4,4> const &maxVal)
{
  for (int r = 0; r < 4; ++r)
      for (int c = 0; c < 4; ++c)
          _spinbox(r, c)->setRange(minVal(r, c), maxVal(r, c));
}

void Double4x4ControlElement::setSingleStep(
    blitz::TinyMatrix<double,4,4> const &step)
{
  for (int r = 0; r < 4; ++r)
      for (int c = 0; c < 4; ++c)
          _spinbox(r, c)->setSingleStep(step(r, c));
}

blitz::TinyMatrix<double,4,4> Double4x4ControlElement::value() const
{
  blitz::TinyMatrix<double,4,4> res;
  for (int r = 0; r < 4; ++r)
      for (int c = 0; c < 4; ++c)
          res(r, c) = _spinbox(r, c)->value();
  return res;
}

std::string Double4x4ControlElement::toStdString() const 
{
  std::stringstream outStream;
  outStream << value();
  return outStream.str();
}

void Double4x4ControlElement::setValue(
    blitz::TinyMatrix<double,4,4> const &value)
{
  // If the matrix is singular, do not set it!!!
  try
  {
    atb::invert(value);
    _lastValidMatrix = value;
  }
  catch (...)
  {
    return;
  }

  bool actualValueChange = false;
  for (int r = 0; r < 4; ++r)
  {
    for (int c = 0; c < 4; ++c)
    {
      if (_spinbox(r, c)->value() != value(r, c))
      {
        actualValueChange = true;
        disconnect(_spinbox(r, c), SIGNAL(valueChanged(double)),
                   this, SIGNAL(valueChanged()));
        _spinbox(r, c)->setValue(value(r, c));
        connect(_spinbox(r, c), SIGNAL(valueChanged(double)),
                SIGNAL(valueChanged()));
      }
    }
  }
  if (actualValueChange) emit valueChanged();
}

void Double4x4ControlElement::setValue(const std::string& value)
{
  blitz::TinyMatrix<double,4,4> mat;
  std::stringstream inStream(value);
  inStream >> mat;
  setValue(mat);
}

void Double4x4ControlElement::fixMatrixAndEmitValueChanged()
{
  try
  {
    atb::invert(value());
    _lastValidMatrix = value();
    emit valueChanged();
  }
  catch (...)
  {
    setValue(_lastValidMatrix);
  }
}
