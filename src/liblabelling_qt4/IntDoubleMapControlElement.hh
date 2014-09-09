/**************************************************************************
**       Title: Widget for editing int double maps
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**************************************************************************/

#ifndef INTDOUBLEMAPCONTROLELEMENT_HH
#define INTDOUBLEMAPCONTROLELEMENT_HH

#include "AbstractControlElement.hh"

#include <map>

class QLabel;
class QDoubleSpinBox;
class QVBoxLayout;

typedef std::map<int,double> IntDoubleMap;

class IntDoubleMapControlElement : public AbstractControlElement
{

  Q_OBJECT
  
public:
  
  IntDoubleMapControlElement(
      const QString& label,
      const IntDoubleMap& value = IntDoubleMap(),
      QWidget* parent = 0);
  ~IntDoubleMapControlElement();

  void setRange(const double min, const double max);
  void setSingleStep(const double step);
  void setValue(const int key, const double value);
  double value(const int key) const;
  void erase(const int key);
  void clear();

  IntDoubleMap value() const;
  std::string toStdString() const;

public slots:
  
  void setValue(const IntDoubleMap& value);
  void setValue(const std::string& value);

private:
  
  size_t find(const int key) const;
  void push_back(const int key, const double value);
  void pop_back();

  double _min, _max, _step;
  std::vector<QVBoxLayout*> _columns;
  std::vector<QLabel*> _keys;
  std::vector<QDoubleSpinBox*> _values;
  
};

#endif
