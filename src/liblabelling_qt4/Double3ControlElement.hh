/**************************************************************************
**       Title: Widget for editing 3-D vectorial floating point numbers
**              in double precision
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**************************************************************************/

#ifndef DOUBLE3CONTROLELEMENT_HH
#define DOUBLE3CONTROLELEMENT_HH

#include "AbstractControlElement.hh"

#include <blitz/array.h>
// #include <blitz/tinyvec-et.h>

class QDoubleSpinBox;

class Double3ControlElement : public AbstractControlElement
{

  Q_OBJECT
  
public:
  
  Double3ControlElement(
      const QString& label,
      const blitz::TinyVector<double,3>& value = -1.0,
      QWidget* parent = 0);
  ~Double3ControlElement();

  void setRange(const blitz::TinyVector<double,3>& min,
                const blitz::TinyVector<double,3>& max);
  void setSingleStep(const blitz::TinyVector<double,3>& step);

  blitz::TinyVector<double,3> value() const;
  std::string toStdString() const;

public slots:
  
  void setValue(const blitz::TinyVector<double,3>& value);
  void setValue(const std::string& value);

private:
  
  blitz::TinyVector<QDoubleSpinBox*,3> _spinbox;
  
};

#endif
