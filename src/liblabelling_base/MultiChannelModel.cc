/**************************************************************************
**       Title: Multi-Channel Model
**    $RCSfile$
**   $Revision: 3883 $ $Name$
**       $Date: 2008-01-21 14:28:13 +0100 (Mon, 21 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    This class holds all channel data
**
**************************************************************************/

#include "MultiChannelModel.hh"

// #include "FloatChannel.hh"
// #include "IntegerChannel.hh"
// #include "RGBChannel.hh"
// #include "AnnotationChannel.hh"
// #include "SplineCurveChannel.hh"
// #include "IRoCSChannel.hh"

#include "MultiChannelModelObserver.hh"

MultiChannelModel::MultiChannelModel()
        : _channels(), _observers(), _activeChannel(-1),
          _shapePx(static_cast<ptrdiff_t>(0)), _elementSizeUm(1.0),
          _modified(false)
{}

MultiChannelModel::~MultiChannelModel()
{
  // Detach all observers from this model
  for (size_t i = 0; i < _observers.size(); ++i) _observers.setModel(NULL);

  // Clear the model
  clear();
}

blitz::TinyVector<ptrdiff_t,3> const &MultiChannelModel::shapePx() const
{
  return _shapePx;
}

blitz::TinyVector<double,3> const &MultiChannelModel::elementSizeUm() const
{
  return _elementSizeUm;
}

void MultiChannelModel::setActiveChannel(int index)
{
  if (_activeChannel == index) return;
  int oldActiveChannel = _activeChannel;
  _activeChannel = index;
  for (std::vector<MultiChannelModelObserver*>::iterator it =
           _observers.begin(); it != observers.end(); ++it)
      (*it)->activeChannelChangedEvent(oldActiveChannel);  
}

int MultiChannelModel::activeChannel() const
{
  return _activeChannel;
}

void MultiChannelModel::setModified(bool modified)
{
  if (_modified == modified) return;
  bool oldModifiedFlag = _modified;
  _modified = modified;
  for (std::vector<MultiChannelModelObserver*>::iterator it =
           _observers.begin(); it != observers.end(); ++it)
      (*it)->modifiedFlagChangedEvent(oldModifiedFlag);
}

bool MultiChannelModel::modified()
{
  return _modified;
}

// FloatChannel *MultiChannelModel::addFloatChannel(
//     Array<float,3> *channelData = NULL)
// {
//   Array<float,3> *ch = channelData;
//   if (channelData == NULL)
//   {
//     if (_channels.size() == 0)
//     {
//       std::cerr << "Warning: Creation of new FloatChannels in an empty model "
//                 << "is only possible with pre-existing data." << std::endl;
//       return NULL;
//     }
//     else ch = new Array<float,3>(_shapePx, _elementSizeUm);
//   }

//   FloatChannel* channel = new FloatChannel(ch, Channel::NeverDeleteData);
//   bool ok = channel->setModel(this);
//   if (!ok)
//   {
//     delete channel;
//     channel = NULL;
//   }
//   else channel->setDataOwnership(Channel::DeleteData);
  
//   return channel;
// }

int MultiChannelModel::addChannel(Channel *channel)
{
  // Check for compatibility with the model
  if (!_isCompatible(channel)) return -1;
  
  // Check whether the channel needs a new name, and generate it
  if (channel->name() == "") channel->setName(_newChannelName());
  
  // Append it to the model
  _channels.push_back(channel);
  
  // Set the Channel's parent to this model
  channel->setModel(this);

  // Set the modified flag of the MultiChannelModel
  setModified(true);
  
  int channelIndex = static_cast<int>(_channels.size()) - 1;

  // Notify the observers of the channel add event
  for (std::vector<MultiChannelModelObserver*>::iterator it =
           _observers.begin(); it != observers.end(); ++it)
      (*it)->channelAddEvent(channelIndex);

  // Make the newly added channel the active channel for editing
  setActiveChannel(channelIndex);

  return channelIndex;
}

int MultiChannelModel::find(Channel *channel) const
{
  int index = 0;
  for (std::vector<Channel*>::const_iterator it = _channels.begin();
       it != _channels.end() && *it != channel; ++it, ++index);
  if (index == _channels.size()) index = -1;
  return index;
}

int MultiChannelModel::findChannelByName(std::string const &name) const
{
  int index = 0;
  for (std::vector<Channel*>::const_iterator it = _channels.begin();
       it != _channels.end() && (*it)->name() != name; ++it, ++index);
  if (index == _channels.size()) index = -1;
  return index;
}

Channel *MultiChannelModel::removeChannel(int index)
{
  if (index < 0 || index >= _channels.size() || !_isRemovable(_channel[index]))
      return NULL;
  
  std::vector<Channel*> iterator it = _channels.begin();
  for (int i = 0; i < index; ++i, ++it);
  Channel *channel = *it;

  // Remove the channel from the model
  _channels.erase(it);
  
  // Detach this model from the channel
  (*it)->setModel(NULL);

  // If the model is not empty set its modified flag
  setModified(_channels.size() != 0);

  // If the model becomes empty reset the shape and element size
  if (_channels.size() == 0)
  {
    setShape(blitz::TinyVector<ptrdiff_t,3>(static_cast<ptrdiff_t>(0)));
    setElementSize(blitz::TinyVector<double,3>(1.0));
  }

  // Notify the observers of the channel removal
  for (std::vector<MultiChannelModelObserver*>::iterator it =
           _observers.begin(); it != observers.end(); ++it)
      (*it)->channelRemoveEvent(index);
  
  return channel;
}

int MultiChannelModel::nChannels(Channel::Types types) const
{
  if (types == Channel::Any) return static_cast<int>(_channels.size());

  int res = 0;
  for (size_t i = 0; i < _channels.size(); ++i)
      if ((_channels[i]->type() & types).any()) ++res;
  return res;
}

Channel const *MultiChannelModel::channel(int index) const
{
  return (index >= 0 && index < _channels.size()) ? _channels[index] : NULL;
}

Channel *MultiChannelModel::channel(int index)
{
  return (index >= 0 && index < _channels.size()) ? _channels[index] : NULL;
}

MultiChannelModel::const_iterator MultiChannelModel::begin() const
{
  return _channels.begin();
}

MultiChannelModel::const_iterator MultiChannelModel::end() const
{
  return _channels.end();
}

MultiChannelModel::iterator MultiChannelModel::begin()
{
  return _channels.begin();
}

MultiChannelModel::iterator MultiChannelModel::end()
{
  return _channels.end();
}

void MultiChannelModel::addObserver(MultiChannelModelObserver *observer)
{
  if (observer == NULL) return;

  // Check whether the observer is already observing this model
  std::vector<MultiChannelObserver*>::iterator it = _observers.begin();
  for (; it != _observers.end() && *it != observer; ++it);
  if (it != _observers.end()) return;

  // OK, not yet, add it
  _observers.push_back(observer);
}

void MultiChannelModel::removeObserver(MultiChannelModelObserver *observer)
{
  if (observer == NULL) return;

  // Check whether the observer is observing this model
  std::vector<MultiChannelObserver*>::iterator it = _observers.begin();
  for (; it != _observers.end() && *it != observer; ++it);

  // Yes, so remove it
  if (it != _observers.end()) _observers.erase(it);
}

bool MultiChannelModel::_isRemovable(Channel *channel)
{
  if (channel == NULL) return false;
  if (find(channel) == -1) return false;
  if (_channels.size() == 1 ||
      (channel->type() != Channel::Float &&
       channel->type() != Channel::Integer &&
       channel->type() != Channel::RGB)) return true;
  return (nChannels(Float | Integer | RGB) != 1);
}

bool MultiChannelModel::_isAddable(Channel *channel)
{
  if (channel == NULL) return false;
  if (find(channel) != -1) return false;
  blitz::TinyVector<ptrdiff_t,3> shapePx(-1);
  blitz::TinyVector<double,3> elementSizeUm(-1.0);
  if (channel->type() == Channel::Float)
  {
    shapePx = static_cast<FloatChannel*>(channel)->data().shape();
    elementSizeUm = static_cast<FloatChannel*>(channel)->data().elementSizeUm();
  }
  if (channel->type() == Channel::Integer)
  {
    shapePx = static_cast<IntegerChannel*>(channel)->data().shape();
    elementSizeUm =
        static_cast<IntegerChannel*>(channel)->data().elementSizeUm();
  }
  if (channel->type() == Channel::RGB)
  {
    shapePx = static_cast<RGBChannel*>(channel)->data().shape();
    elementSizeUm = static_cast<RGBChannel*>(channel)->data().elementSizeUm();
  }
  return ((_channels.size() == 0 &&
           blitz::all(shapePx > 0 && elementSizeUm > 0.0)) ||
          (_channels.size() != 0 &&
           (blitz::all(shapePx == _shapePx &&
                       elementSizeUm == _elementSizeUm)) ||
           blitz::all(shapePx == -1)));
}

std::string MultiChannelModel::_newChannelName() const
{
  std::string name;
  bool nameUnique = true;
  int i = 0;
  do 
  {
    std::stringstream nameStream;
    nameStream << "channel_" << i;
    name = nameStream.str();
    nameUnique = true;
    for (std::vector<ChannelSpecs*>::const_iterator it = begin();
         nameUnique && it != end(); ++it) 
    {
      if (name == (*it)->name()) nameUnique = false;
    }
    ++i;
  }
  while (!nameUnique);
  return name;
}

bool MultiChannelModel::_channelNamesUnique() const
{
  std::vector<ChannelSpecs*>::const_iterator it = begin();
  std::vector<std::string> readNames;
  for (; it != end(); ++it) 
  {
    bool unique = true;
    for (std::vector<std::string>::iterator it2 = readNames.begin();
         unique && it2 != readNames.end(); ++it2) 
    {
      if (*it2 == (*it)->name()) unique = false;
    }
    if (!unique) return false;
    readNames.push_back((*it)->name());
  }
  return true;
}

