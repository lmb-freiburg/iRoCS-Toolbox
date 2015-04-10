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

#ifndef CHANNELSELECTIONCONTROLELEMENT_HH
#define CHANNELSELECTIONCONTROLELEMENT_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "AbstractControlElement.hh"

#include "ChannelSpecs.hh"

#include <vector>

class MultiChannelModel;

class QComboBox;
    
class ChannelSelectionControlElement : public AbstractControlElement
{
  
  Q_OBJECT

public:

  ChannelSelectionControlElement(
      const QString& label, MultiChannelModel *model,
      ChannelSpecs::ChannelTypes channelTypes = ChannelSpecs::Any,
      QWidget* parent = 0);

  ~ChannelSelectionControlElement();
  
  bool contains(std::string const &channelName) const;
  bool contains(ChannelSpecs *channel) const;
  int indexOf(ChannelSpecs *channel) const;
  int nChannels() const;

  bool isEditable() const;
  void setEditable(bool editable);

  ChannelSpecs *selectedChannel() const;
  ChannelSpecs *channel(int index) const;
  ChannelSpecs *channel(std::string const &name) const;

  int value() const;
  std::string toStdString() const;

signals:
        
  void activated(int index);
  void activated(const QString& text);
  void currentIndexChanged(int index);
  void currentIndexChanged(const QString& text);
  void editTextChanged(const QString& text);
  void editingFinished();
  void highlighted(int index);
  void highlighted(const QString& text);
  void channelListEmpty(bool);

public slots:
  
  void setValue(ChannelSpecs *channel);
  void setValue(const int value);
  void setValue(const std::string& value);
  
private slots:
  
  void appendChannel(ChannelSpecs *channel);
  void removeChannel(ChannelSpecs *channel);
  void updateChannelNamesAndIcons();
  void updateChannels();

private:

  ChannelSpecs::ChannelTypes _channelTypes; 
  MultiChannelModel *p_model;
  QComboBox* p_combobox;
  std::vector<ChannelSpecs*> _channels;

};

#endif
