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

#ifndef DOUBLE4X4CONTROLELEMENT_HH
#define DOUBLE4X4CONTROLELEMENT_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "AbstractControlElement.hh"

#include <blitz/array.h>

#include <libArrayToolbox/TypeTraits.hh>

class QDoubleSpinBox;

class Double4x4ControlElement : public AbstractControlElement
{

  Q_OBJECT
  
public:
  
  Double4x4ControlElement(
      QString const &label,
      blitz::TinyMatrix<double,4,4> const &value =
      atb::traits< blitz::TinyMatrix<double,4,4> >::one,
      QWidget *parent = 0);
  ~Double4x4ControlElement();
  
  void setRange(blitz::TinyMatrix<double,4,4> const &minVal,
                blitz::TinyMatrix<double,4,4> const &maxVal);
  void setSingleStep(blitz::TinyMatrix<double,4,4> const &step);

  blitz::TinyMatrix<double,4,4> value() const;
  std::string toStdString() const;

public slots:
  
  void setValue(blitz::TinyMatrix<double,4,4> const &value);
  void setValue(std::string const &value);

private slots:
  
  void fixMatrixAndEmitValueChanged();

private:
  
  blitz::TinyMatrix<QDoubleSpinBox*,4,4> _spinbox;
  blitz::TinyMatrix<double,4,4> _lastValidMatrix;
  
};

#endif
