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

#ifndef FILENAMESELECTIONCONTROLELEMENT_HH
#define FILENAMESELECTIONCONTROLELEMENT_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "AbstractControlElement.hh"

class QLineEdit;

class FileNameSelectionControlElement : public AbstractControlElement
{

  Q_OBJECT
  
public:
  
  FileNameSelectionControlElement(
      const QString& label, const std::string& value = std::string(),
      bool onlyExistingFiles = false, const QString& filter = "",
      QWidget* parent = 0);
  ~FileNameSelectionControlElement();

  std::string value() const;
  std::string toStdString() const;

public slots:
  
  void setValue(const std::string& value);

signals:
        
  void valueChanged(QString const &filename);
  void valueChanged(std::string const &filename);

private slots:
  
  void openFileSelector();
  void emitValueChange();

private:
  
  bool _onlyExisting;
  QString _filter;
  QLineEdit* p_lineedit;
  
};

#endif
