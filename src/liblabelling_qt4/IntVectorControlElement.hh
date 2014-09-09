/**************************************************************************
**       Title: Widget for editing vectorial integer numbers
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**************************************************************************/

#ifndef INTVECTORCONTROLELEMENT_HH
#define INTVECTORCONTROLELEMENT_HH

#include "AbstractControlElement.hh"

class QSpinBox;

#include <vector>

class IntVectorControlElement : public AbstractControlElement
{

  Q_OBJECT
  
public:
  
  IntVectorControlElement(
      const QString& label,
      const std::vector<int>& value = std::vector<int>(),
      QWidget* parent = 0);
  ~IntVectorControlElement();

  void setRange(const int min, const int max);
  void setLength(size_t length);
  size_t length() const;
  void push_back(const int value);
  void pop_back();

  std::vector<int> value() const;
  std::string toStdString() const;

public slots:
  
  void setValue(const std::vector<int>& value);
  void setValue(const std::string& value);

private:
  
  int _min, _max;
  std::vector<QSpinBox*> _spinbox;
  
};

#endif
