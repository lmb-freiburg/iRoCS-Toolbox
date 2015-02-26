/**************************************************************************
**       Title: Widget for editing 4x4 double precision matrices
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**************************************************************************/

#ifndef DOUBLE4X4CONTROLELEMENT_HH
#define DOUBLE4X4CONTROLELEMENT_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "AbstractControlElement.hh"

#include <blitz/array.h>

#include <libArrayToolbox/TypeTraits.hh>

class QDoubleSpinBox;

class Double4x4ControlElement : public AbstractControlElement
{

  Q_OBJECT
  
public:
  
  Double4x4ControlElement(
      QString const &label,
      blitz::TinyMatrix<double,4,4> const &value =
      atb::traits< blitz::TinyMatrix<double,4,4> >::one,
      QWidget *parent = 0);
  ~Double4x4ControlElement();
  
  void setRange(blitz::TinyMatrix<double,4,4> const &minVal,
                blitz::TinyMatrix<double,4,4> const &maxVal);
  void setSingleStep(blitz::TinyMatrix<double,4,4> const &step);

  blitz::TinyMatrix<double,4,4> value() const;
  std::string toStdString() const;

public slots:
  
  void setValue(blitz::TinyMatrix<double,4,4> const &value);
  void setValue(std::string const &value);

private slots:
  
  void fixMatrixAndEmitValueChanged();

private:
  
  blitz::TinyMatrix<QDoubleSpinBox*,4,4> _spinbox;
  blitz::TinyMatrix<double,4,4> _lastValidMatrix;
  
};

#endif
