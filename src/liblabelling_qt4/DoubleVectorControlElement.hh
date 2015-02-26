/**************************************************************************
**       Title: Widget for editing vectorial double precision
**              floating point numbers
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**************************************************************************/

#ifndef DOUBLEVECTORCONTROLELEMENT_HH
#define DOUBLEVECTORCONTROLELEMENT_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "AbstractControlElement.hh"

#include <vector>

class QDoubleSpinBox;

class DoubleVectorControlElement : public AbstractControlElement
{

  Q_OBJECT
  
public:
  
  DoubleVectorControlElement(
      const QString& label,
      const std::vector<double>& value = std::vector<double>(),
      QWidget* parent = 0);
  ~DoubleVectorControlElement();

  size_t length() const;
  void push_back(const double value);
  void pop_back();

  std::vector<double> value() const;
  std::string toStdString() const;

public slots:
  
  void setRange(const double min, const double max);
  void setSingleStep(const double step);
  void setLength(size_t length);
  void setValue(const std::vector<double>& value);
  void setValue(const std::string& value);

private:
  
  double _min, _max, _step;
  std::vector<QDoubleSpinBox*> _spinbox;
  
};

#endif
