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

#ifndef STRINGSELECTIONCONTROLELEMENT_HH
#define STRINGSELECTIONCONTROLELEMENT_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "AbstractControlElement.hh"

class QComboBox;
class QStringList;

class StringSelectionControlElement : public AbstractControlElement
{
  
  Q_OBJECT

public:

  StringSelectionControlElement(
      const QString& label, QStringList const &elements, QWidget* parent = 0);
  ~StringSelectionControlElement();
  
  int value() const;
  int nValues() const;
  std::string toStdString() const;
  
signals:
        
  void activated(int index);
  void activated(const QString& text);
  void currentIndexChanged(int index);
  void currentIndexChanged(const QString& text);
  void highlighted(int index);
  void highlighted(const QString& text);
  void valueChanged(int index);

public slots:
  
  void setValue(int index);
  void setValue(const std::string& value);
  
private:

  QComboBox* p_combobox;

};

#endif
