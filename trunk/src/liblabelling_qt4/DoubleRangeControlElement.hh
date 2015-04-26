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

#ifndef DOUBLERANGECONTROLELEMENT_HH
#define DOUBLERANGECONTROLELEMENT_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

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
