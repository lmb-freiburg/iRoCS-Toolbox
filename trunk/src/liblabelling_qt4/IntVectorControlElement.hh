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

#ifndef INTVECTORCONTROLELEMENT_HH
#define INTVECTORCONTROLELEMENT_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

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
