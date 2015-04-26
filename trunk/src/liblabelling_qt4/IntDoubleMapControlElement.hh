/**************************************************************************
 *
 * This file belongs to the iRoCS Toolbox.
 *
 * Copyright (C) 2015 Thorsten Falk
 *
 *        Image Analysis Lab, University of Freiburg, Germany
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 **************************************************************************/

#ifndef INTDOUBLEMAPCONTROLELEMENT_HH
#define INTDOUBLEMAPCONTROLELEMENT_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

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
