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

#ifndef INTCONTROLELEMENT_HH
#define INTCONTROLELEMENT_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "AbstractControlElement.hh"

class QSpinBox;

class IntControlElement : public AbstractControlElement
{

  Q_OBJECT
  
public:
  
  IntControlElement(
      const QString& label, const int value = -1, QWidget* parent = 0);
  ~IntControlElement();

  void setRange(const int min, const int max);

/*======================================================================*/
/*! 
 *   If specified, the given text is shown if the spin box is at its
 *   minimum value instead of that value. If you pass a NULL (empty) string
 *   it will be reset to show the value again.
 *
 *   \param text The text to output instead of the spinbox's minimum value
 */
/*======================================================================*/
  void setSpecialValueText(QString const &text);

/*======================================================================*/
/*! 
 *   If specified, the given text is appended to the value in the spinbox.
 *
 *   \param text The text to append (e.g. unit)
 */
/*======================================================================*/
  void setSuffix(QString const &text);

  int value() const;
  std::string toStdString() const;

signals:
  void valueChanged(int);
  
public slots:
  
  void setValue(const int value);
  void setValue(const std::string& value);

private:
  
  QSpinBox* p_spinbox;
  
};

#endif
