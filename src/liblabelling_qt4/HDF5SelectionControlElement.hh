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

#ifndef HDF5SELECTIONCONTROLELEMENT_HH
#define HDF5SELECTIONCONTROLELEMENT_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "AbstractControlElement.hh"

#include "ChannelSpecs.hh"

class QLineEdit;

class MultiChannelModel;

class HDF5SelectionControlElement : public AbstractControlElement
{

  Q_OBJECT
  
public:
  
  HDF5SelectionControlElement(
      const QString& label, QString const &filename,
      const std::string& value = std::string(),
      MultiChannelModel *model = NULL,
      ChannelSpecs::ChannelTypes types = ChannelSpecs::Any,
      QWidget* parent = NULL);
  ~HDF5SelectionControlElement();

  QString const &fileName() const;

  std::string value() const;
  std::string toStdString() const;

public slots:

  void setFileName(QString const &filename);
  void setValue(const std::string& value);

private slots:
  
  void openHDF5SelectionDialog();

private:
  
  QString _filename;
  MultiChannelModel *p_model;
  ChannelSpecs::ChannelTypes _channelTypes;
  QLineEdit* p_lineedit;
  QWidget *p_parent;
  
};

#endif
