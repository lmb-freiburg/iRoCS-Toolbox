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
