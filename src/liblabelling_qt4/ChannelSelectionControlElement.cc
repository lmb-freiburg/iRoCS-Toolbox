/**************************************************************************
**       Title: Widget for choosing a loaded labelling_qt4 channel
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**************************************************************************/

#include "ChannelSelectionControlElement.hh"

#include <QtGui/QHBoxLayout>
#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>

#include "MultiChannelModel.hh"

ChannelSelectionControlElement::ChannelSelectionControlElement(
    const QString& label, MultiChannelModel *model,
    ChannelSpecs::ChannelTypes channelTypes, QWidget* parent)
        : AbstractControlElement(label, parent), _channelTypes(channelTypes),
          p_model(model)
{
  p_combobox = new QComboBox;
  p_combobox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  p_combobox->setInsertPolicy(QComboBox::InsertAtCurrent);
  p_layout->addWidget(p_combobox);

  connect(p_model, SIGNAL(channelAdded(ChannelSpecs *)),
          SLOT(appendChannel(ChannelSpecs *)));
  connect(p_model, SIGNAL(channelRemoved(ChannelSpecs *)),
          SLOT(removeChannel(ChannelSpecs *)));
  connect(p_model, SIGNAL(channelNamesOrIconsChanged()),
          SLOT(updateChannelNamesAndIcons()));  

  connect(p_combobox, SIGNAL(activated(int)), SIGNAL(activated(int)));
  connect(p_combobox, SIGNAL(activated(const QString&)),
          SIGNAL(activated(const QString&)));
  connect(p_combobox, SIGNAL(currentIndexChanged(int)),
          SIGNAL(currentIndexChanged(int)));
  connect(p_combobox, SIGNAL(currentIndexChanged(const QString&)),
          SIGNAL(currentIndexChanged(const QString&)));
  connect(p_combobox, SIGNAL(highlighted(int)), SIGNAL(highlighted(int)));
  connect(p_combobox, SIGNAL(highlighted(const QString&)),
          SIGNAL(highlighted(const QString&)));

  updateChannels();
}

ChannelSelectionControlElement::~ChannelSelectionControlElement()
{}

bool ChannelSelectionControlElement::contains(std::string const &channelName)
    const
{
  return (p_combobox->findText(channelName.c_str()) != -1);
}

bool ChannelSelectionControlElement::contains(ChannelSpecs *channel) const
{
  for (size_t i = 0; i < _channels.size(); ++i)
      if (_channels[i] == channel) return true;
  return false;
}

int ChannelSelectionControlElement::indexOf(ChannelSpecs *channel) const
{
  for (size_t i = 0; i < _channels.size(); ++i)
      if (_channels[i] == channel) return static_cast<int>(i);
  return -1;
}

int ChannelSelectionControlElement::nChannels() const
{
  return static_cast<int>(_channels.size());
}

bool ChannelSelectionControlElement::isEditable() const
{
  return p_combobox->isEditable();
}

void ChannelSelectionControlElement::setEditable(bool editable)
{
  p_combobox->setEditable(editable);
  if (editable)
      connect(p_combobox->lineEdit(), SIGNAL(editingFinished()),
              SIGNAL(editingFinished()));
}

ChannelSpecs *ChannelSelectionControlElement::selectedChannel() const
{
  if (p_combobox->currentIndex() < 0 ||
      p_combobox->currentIndex() > static_cast<int>(_channels.size()))
      return NULL;
  return _channels[p_combobox->currentIndex()];
}

ChannelSpecs *ChannelSelectionControlElement::channel(int index) const
{
  if (index < 0 || index >= static_cast<int>(_channels.size())) return NULL;
  return _channels[index];
}

ChannelSpecs *ChannelSelectionControlElement::channel(std::string const &name)
    const
{
  for (size_t i = 0; i < _channels.size(); ++i)
      if (_channels[i]->name() == name) return _channels[i];
  return NULL;
}

int ChannelSelectionControlElement::value() const
{
  return p_combobox->currentIndex();
}

std::string ChannelSelectionControlElement::toStdString() const
{
  return p_combobox->currentText().toStdString();
}
  
void ChannelSelectionControlElement::setValue(ChannelSpecs *channel)
{
  if (_channels[p_combobox->currentIndex()] == channel) return;
  for (size_t i = 0; i < _channels.size(); ++i)
  {
    if (_channels[i] == channel)
    {
      p_combobox->setCurrentIndex(static_cast<int>(i));
      return;
    }
  }
}

void ChannelSelectionControlElement::setValue(const int value)
{
  if (p_combobox->currentIndex() == value) return;
  if (value >= 0 && value < p_combobox->count())
      p_combobox->setCurrentIndex(value);
}

void ChannelSelectionControlElement::setValue(const std::string& value)
{
  int index = p_combobox->findText(value.c_str());
  setValue(index);
}

void ChannelSelectionControlElement::appendChannel(ChannelSpecs *channel)
{
#ifdef DEBUG
  std::cerr << __FILE__ << ":" << __LINE__ << ": appendChannel(" << channel
            << " (" << channel->name() << ")) called. channel types = "
            << _channelTypes << ", channels = [ " << std::flush;
  for (size_t i = 0; i < _channels.size(); ++i)
      std::cerr << _channels[i] << " (" << _channels[i]->name() << ") "
                << std::flush;
  std::cerr << "]" << std::endl;
#endif

  if (!_channelTypes.testFlag(channel->channelType())) return;
  _channels.push_back(channel);
  p_combobox->insertItem(
      p_combobox->count(), channel->icon(), channel->name().c_str());
  p_combobox->setEnabled(true);
  if (_channels.size() == 1) emit channelListEmpty(false);

#ifdef DEBUG
  std::cerr << __FILE__ << ":" << __LINE__ << ": appendChannel(" << channel
            << " (" << channel->name() << ")) finished. channel types = "
            << _channelTypes << ", channels = [ " << std::flush;
  for (size_t i = 0; i < _channels.size(); ++i)
      std::cerr << _channels[i] << " (" << _channels[i]->name() << ") "
                << std::flush;
  std::cerr << "]" << std::endl;
#endif
}

void ChannelSelectionControlElement::removeChannel(ChannelSpecs *channel)
{
#ifdef DEBUG
  std::cerr << __FILE__ << ":" << __LINE__ << ": removeChannel(" << channel
            << ") called. channel types = "
            << _channelTypes << ", channels = [ " << std::flush;
  for (size_t i = 0; i < _channels.size(); ++i)
      std::cerr << _channels[i] << " ("
                << ((_channels[i] == channel) ? std::string("<REMOVED>") :
                    _channels[i]->name()) << ") " << std::flush;
  std::cerr << "]" << std::endl;
#endif

  int index = 0;
  while (index < static_cast<int>(_channels.size()) &&
         _channels[index] != channel) ++index;
  if (index == static_cast<int>(_channels.size())) return;
  int selectedIndex = p_combobox->currentIndex();
  for (size_t i = index; i < _channels.size() - 1; ++i)
      _channels[i] = _channels[i + 1];
  _channels.pop_back();
  p_combobox->removeItem(index);
  if (selectedIndex > index) p_combobox->setCurrentIndex(selectedIndex - 1);
  p_combobox->setEnabled(_channels.size() != 0);
  if (_channels.size() == 0) emit channelListEmpty(true);

#ifdef DEBUG
  std::cerr << __FILE__ << ":" << __LINE__ << ": removeChannel(" << channel
            << ") finished. channel types = "
            << _channelTypes << ", channels = [ " << std::flush;
  for (size_t i = 0; i < _channels.size(); ++i)
      std::cerr << _channels[i] << " (" << _channels[i]->name() << ") "
                << std::flush;
  std::cerr << "]" << std::endl;
#endif
}

void ChannelSelectionControlElement::updateChannelNamesAndIcons()
{
#ifdef DEBUG
  std::cerr << __FILE__ << ":" << __LINE__ << ": updateChannelNamesAndIcons() "
            << "called. channel types = "
            << _channelTypes << ", channels = [ " << std::flush;
  for (size_t i = 0; i < _channels.size(); ++i)
      std::cerr << _channels[i] << " (" << _channels[i]->name() << ") "
                << std::flush;
  std::cerr << "]" << std::endl;
#endif

  for (int i = 0; i < p_combobox->count(); ++i)
  {
    p_combobox->setItemIcon(i, _channels[i]->icon());
    p_combobox->setItemText(i, _channels[i]->name().c_str());
  }

#ifdef DEBUG
  std::cerr << __FILE__ << ":" << __LINE__ << ": updateChannelNamesAndIcons() "
            << "finished. channel types = "
            << _channelTypes << ", channels = [ " << std::flush;
  for (size_t i = 0; i < _channels.size(); ++i)
      std::cerr << _channels[i] << " (" << _channels[i]->name() << ") "
                << std::flush;
  std::cerr << "]" << std::endl;
#endif
}

void ChannelSelectionControlElement::updateChannels()
{
  disconnect(p_combobox, SIGNAL(activated(int)),
             this, SIGNAL(activated(int)));
  disconnect(p_combobox, SIGNAL(activated(const QString&)),
             this, SIGNAL(activated(const QString&)));
  disconnect(p_combobox, SIGNAL(currentIndexChanged(int)),
             this, SIGNAL(currentIndexChanged(int)));
  disconnect(p_combobox, SIGNAL(currentIndexChanged(const QString&)),
             this, SIGNAL(currentIndexChanged(const QString&)));
  disconnect(p_combobox, SIGNAL(editTextChanged(const QString&)),
             this, SIGNAL(editTextChanged(const QString&)));
  disconnect(p_combobox, SIGNAL(highlighted(int)),
             this, SIGNAL(highlighted(int)));
  disconnect(p_combobox, SIGNAL(highlighted(const QString&)),
             this, SIGNAL(highlighted(const QString&)));
  if (p_combobox->isEditable())
      disconnect(p_combobox->lineEdit(), SIGNAL(editingFinished()),
                 this, SIGNAL(editingFinished()));
  
  size_t nChannelsOld = _channels.size();
  
  p_combobox->clear();
  _channels.clear();
  int index = 0;
  int i = 0;
  for (std::vector<ChannelSpecs*>::const_iterator it = p_model->begin();
       it != p_model->end(); ++it)
  {
    if (_channelTypes.testFlag((*it)->channelType()))
    {
      p_combobox->addItem((*it)->icon(), (*it)->name().c_str());
      _channels.push_back(*it);
      if (*it == p_model->selectedChannel()) index = i;
      ++i;
    }
  }
  if (nChannelsOld != 0 && _channels.size() == 0)
      emit channelListEmpty(true);
  if (nChannelsOld == 0 && _channels.size() != 0)
      emit channelListEmpty(false);
  p_combobox->setCurrentIndex(index);
  
  connect(p_combobox, SIGNAL(activated(int)), SIGNAL(activated(int)));
  connect(p_combobox, SIGNAL(activated(const QString&)),
          SIGNAL(activated(const QString&)));
  connect(p_combobox, SIGNAL(currentIndexChanged(int)),
          SIGNAL(currentIndexChanged(int)));
  connect(p_combobox, SIGNAL(currentIndexChanged(const QString&)),
          SIGNAL(currentIndexChanged(const QString&)));
  connect(p_combobox, SIGNAL(editTextChanged(const QString&)),
          SIGNAL(editTextChanged(const QString&)));
  connect(p_combobox, SIGNAL(highlighted(int)), SIGNAL(highlighted(int)));
  connect(p_combobox, SIGNAL(highlighted(const QString&)),
          SIGNAL(highlighted(const QString&)));
  if (p_combobox->isEditable())
      connect(p_combobox->lineEdit(), SIGNAL(editingFinished()),
              SIGNAL(editingFinished()));
  p_combobox->setEnabled(_channels.size() != 0);
  emit channelListEmpty(_channels.size() == 0);
}

