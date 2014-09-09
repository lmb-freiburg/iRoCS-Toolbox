/**************************************************************************
**       Title: Widget for editing floating point ranges in double 
**              precision
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**************************************************************************/

#ifndef DOUBLERANGECONTROLELEMENT_HH
#define DOUBLERANGECONTROLELEMENT_HH

#include "AbstractControlElement.hh"

class QDoubleSpinBox;

class DoubleRangeControlElement : public AbstractControlElement
{

  Q_OBJECT
  
public:
  
  DoubleRangeControlElement(
      const QString& label, double lbound = 0.0, double ubound = 0.0,
      QWidget* parent = 0);
  ~DoubleRangeControlElement();

  void setRange(const double min, const double max);
  void setSingleStep(const double step);

  double lowerBound() const;
  double upperBound() const;
  std::string toStdString() const;

signals:

  void valueChanged(double, double);

public slots:
  
  void setLowerBound(double lbound);
  void setUpperBound(double ubound);  
  void setValue(double lbound, double ubound);
  void setValue(const std::string& value);

private slots:
  
  void setMinimumUpperBound(double);
  void setMaximumLowerBound(double);
  void emitValueChange();

private:
  
  QDoubleSpinBox *p_lbspinbox, *p_ubspinbox;
  
};

#endif
