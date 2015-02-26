/**************************************************************************
**       Title: Widget for choosing a loaded labelling_qt4 channel
**    $RCSfile$
**   $Revision:$ $Name$
**       $Date:$
**   Copyright: GPL $Author:$
** Description:
**
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
