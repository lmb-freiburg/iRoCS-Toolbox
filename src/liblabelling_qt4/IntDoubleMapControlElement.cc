/**************************************************************************
**       Title: Widget for editing int double maps
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**************************************************************************/

#include "IntDoubleMapControlElement.hh"

#include <QtGui/QLabel>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QVBoxLayout>

#include <sstream>
#include <limits>

IntDoubleMapControlElement::IntDoubleMapControlElement(
    const QString& label, const IntDoubleMap& value,
    QWidget* parent)
        : AbstractControlElement(label, parent),
          _min(-std::numeric_limits<double>::infinity()),
          _max(std::numeric_limits<double>::infinity()),
          _step(0.1)
{
  _keys.resize(value.size());
  _values.resize(value.size());
  int i = 0;
  for (IntDoubleMap::const_iterator it = value.begin();
       it != value.end(); ++it, ++i)
  {
    _columns[i] = new QVBoxLayout;
    _keys[i]= new QLabel(QString::number(it->first));
    _columns[i]->addWidget(_keys[i]);
    _values[i] = new QDoubleSpinBox;
    _values[i]->setRange(_min, _max);
    _values[i]->setDecimals(5);
    _values[i]->setSingleStep(_step);
    _values[i]->setValue(it->second);
    _values[i]->setMinimumWidth(80);
    _columns[i]->addWidget(_values[i]);
    p_layout->addLayout(_columns[i]);
    connect(_values[i], SIGNAL(valueChanged(double)), SIGNAL(valueChanged()));
  }
}

IntDoubleMapControlElement::~IntDoubleMapControlElement()
{}

void IntDoubleMapControlElement::setRange(const double min, const double max)
{
  _min = min;
  _max = max;
  for (size_t d = 0; d < _values.size(); ++d) _values[d]->setRange(min, max);
}

void IntDoubleMapControlElement::setSingleStep(const double step)
{
  _step = step;
  for (size_t d = 0; d < _values.size(); ++d) _values[d]->setSingleStep(step);
}

void IntDoubleMapControlElement::setValue(const int key, const double value)
{
  size_t i = find(key);
  if (i == _keys.size()) push_back(key, value);
  else _values[i]->setValue(value);
}

double IntDoubleMapControlElement::value(const int key) const
{
  size_t i = find(key);
  if (i == _keys.size()) return -1.0;
  return _values[i]->value();
}

void IntDoubleMapControlElement::erase(const int key)
{
  size_t i = find(key);
  if (i == _keys.size()) return;

  _columns[i]->setParent(0);
  delete _columns[i];

  std::vector<QVBoxLayout*>::iterator itC = _columns.begin();
  std::vector<QLabel*>::iterator itK = _keys.begin();
  std::vector<QDoubleSpinBox*>::iterator itV = _values.begin();
  for (size_t j = 0; j < i; ++j, ++itC, ++itK, ++itV);
  _columns.erase(itC);
  _keys.erase(itK);
  _values.erase(itV);
}

void IntDoubleMapControlElement::clear()
{
  for (size_t i = 0; i < _keys.size(); ++i)
  {
    _columns[i]->setParent(0);
    delete _columns[i];
  }
  _columns.clear();
  _keys.clear();
  _values.clear();
}

IntDoubleMap IntDoubleMapControlElement::value() const
{
  IntDoubleMap value;
  for (size_t d = 0; d < _keys.size(); ++d)
      value[_keys[d]->text().toInt()] = _values[d]->value();
  return value;
}

std::string IntDoubleMapControlElement::toStdString() const 
{
  std::stringstream outStream;
  outStream << "( ";
  for (size_t d = 0; d < _keys.size() - 1; ++d)
      outStream << _keys[d]->text().toStdString() << "="
                << _values[d]->value() << ", ";
  outStream << _keys.back()->text().toStdString() << "="
            << _values.back()->value() << " )";
  return outStream.str();
}

void IntDoubleMapControlElement::setValue(const IntDoubleMap& value)
{
  while (_keys.size() > value.size()) pop_back();
  while (_keys.size() < value.size()) push_back(-1,-1.0);
  // This line really does the job
  size_t i = 0;
  for (IntDoubleMap::const_iterator it = value.begin();
       it != value.end(); ++it, ++i)
  {
    _keys[i]->setText(QString::number(it->first));
    _values[i]->setValue(it->second);
  }
}

void IntDoubleMapControlElement::setValue(const std::string& value)
{
  IntDoubleMap tmp;
  std::stringstream inStream(value);
  inStream.ignore(1);
  int key;
  double val;
  while (!inStream.fail() && !inStream.eof())
  {
    inStream >> key;
    inStream.ignore(1);
    inStream >> val;
    inStream.ignore(1);
    tmp[key] = val;
  }
  setValue(tmp);
}

size_t IntDoubleMapControlElement::find(const int key) const 
{
  size_t i = 0;
  while (i < _keys.size() && _keys[i]->text().toInt() != key) ++i;
  return i;
}

void IntDoubleMapControlElement::push_back(const int key, const double value) 
{
  _columns.push_back(new QVBoxLayout);
  _keys.push_back(new QLabel(QString::number(key)));
  _columns.back()->addWidget(_keys.back());
  _values.push_back(new QDoubleSpinBox);
  _values.back()->setRange(_min, _max);
  _values.back()->setSingleStep(_step);
  _values.back()->setDecimals(5);
  _values.back()->setValue(value);
  _values.back()->setMinimumWidth(80);
  _columns.back()->addWidget(_values.back());
  p_layout->addLayout(_columns.back());
  connect(_values.back(), SIGNAL(valueChanged(double)),
          SIGNAL(valueChanged()));  
}

void IntDoubleMapControlElement::pop_back()
{
  _columns.back()->setParent(0);
  delete _columns.back();
  _columns.pop_back();
  _keys.pop_back();
  _values.pop_back();
}

