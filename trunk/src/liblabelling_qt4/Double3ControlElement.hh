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

#ifndef DOUBLE3CONTROLELEMENT_HH
#define DOUBLE3CONTROLELEMENT_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "AbstractControlElement.hh"

#include <blitz/array.h>

class QDoubleSpinBox;

class Double3ControlElement : public AbstractControlElement
{

  Q_OBJECT
  
public:
  
  Double3ControlElement(
      const QString& label,
      const blitz::TinyVector<double,3>& value = -1.0,
      QWidget* parent = 0);
  ~Double3ControlElement();

  void setRange(const blitz::TinyVector<double,3>& min,
                const blitz::TinyVector<double,3>& max);
  void setSingleStep(const blitz::TinyVector<double,3>& step);

  blitz::TinyVector<double,3> value() const;
  std::string toStdString() const;

public slots:
  
  void setValue(const blitz::TinyVector<double,3>& value);
  void setValue(const std::string& value);

private:
  
  blitz::TinyVector<QDoubleSpinBox*,3> _spinbox;
  
};

#endif
