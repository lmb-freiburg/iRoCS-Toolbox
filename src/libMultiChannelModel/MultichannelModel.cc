/**************************************************************************
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

#include "MultichannelModel.hh"
#include "MultichannelModelObserver.hh"
#include "Channel.hh"

#include <cstddef>

namespace iRoCS {

#ifdef DEBUG_VERBOSE_XML
  std::string MultichannelModel::printState() const {
    std::stringstream stateStream;
    stateStream << "_channels=\"";
    if (_channels.size() != 0) {
      const_iterator it = _channels.begin();
      stateStream << *it;
      ++it;
      for (; it != _channels.end(); ++it) stateStream << "," << *it;
    }
    stateStream << "\" _observers=\"";
    if (_observers.size() != 0) {
      std::set<MultichannelModelObserver*>::const_iterator it =
        _observers.begin();
      stateStream << *it;
      ++it;
      for (; it != _observers.end(); ++it) stateStream << "," << *it;
    }
    stateStream << "\"";
    return stateStream.str();
  }
#endif

  MultichannelModel::MultichannelModel()
    : Observable(), _observers(), _channels(), _modified(false) {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<MultichannelModel@" << this << "::MultichannelModel "
      << printState() << " />" << std::endl;
#endif
  }

  MultichannelModel::~MultichannelModel() {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<MultichannelModel@" << this << "::~MultichannelModel "
      << printState() << ">" << std::endl;
#endif

    while (_observers.size() != 0) delete *_observers.begin();
    clear();

#ifdef DEBUG_VERBOSE_XML
    std::cerr << "</MultichannelModel@" << this << "::~MultichannelModel>"
      << std::endl;
#endif
  }

  void MultichannelModel::addChannel(Channel *channel) {
    if (channel == NULL || _channels.find(channel) != _channels.end()) return;

#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<MultichannelModel@" << this << "::addChannel channel=\""
      << channel << "\" " << printState() << ">" << std::endl;
#endif

    std::cout << "Adding channel '" << channel->name()
      << "' to MultichannelModel" << std::endl;
    std::cout << "  element size in micrometers = " << channel->elementSizeUm()
      << std::endl;
    std::cout << "  transformation = " << channel->transformation()
      << std::endl;
    std::cout << "  Channel bounding box = "
      << channel->boundingBoxLowerBoundUm() << " - "
      << channel->boundingBoxUpperBoundUm() << std::endl;
    channel->setName(ensureValidName(channel));
    _channels.insert(channel);
    channel->setModel(this);
    for (std::set<MultichannelModelObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateAddChannel(channel);
    setModified(true);
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "</MultichannelModel@" << this << "::addChannel>" << std::endl;
#endif
  }

  void MultichannelModel::removeChannel(Channel *channel) {
    if (channel == NULL || _channels.find(channel) == _channels.end()) return;

#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<MultichannelModel@" << this << "::removeChannel channel=\""
      << channel << "\" " << printState() << ">" << std::endl;
#endif

    for (std::set<MultichannelModelObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateRemoveChannel(channel);
    _channels.erase(channel);
    channel->setModel(NULL);
    setModified(_channels.size() > 0);

#ifdef DEBUG_VERBOSE_XML
    std::cerr << "</MultichannelModel@" << this << "::removeChannel>"
      << std::endl;
#endif
  }

  size_t MultichannelModel::nChannels(Channel::Types types) const {
    if (types == Channel::Any) return _channels.size();

    size_t res = 0;
    for (const_iterator it = begin(); it != end(); ++it)
      if (types.testFlag((*it)->type())) ++res;
    return res;
  }

  MultichannelModel::iterator MultichannelModel::begin() {
    return _channels.begin();
  }

  MultichannelModel::const_iterator MultichannelModel::begin() const {
    return _channels.begin();
  }

  MultichannelModel::iterator MultichannelModel::end() {
    return _channels.end();
  }
  MultichannelModel::const_iterator MultichannelModel::end() const {
    return _channels.end();
  }

  void MultichannelModel::clear() {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<MultichannelModel@" << this << "::clear " << printState()
      << ">" << std::endl;
#endif

    while (_channels.size() != 0) {
#ifdef DEBUG_VERBOSE_XML
      std::cerr << "delete " << *_channels.begin() << std::endl;
#endif
      delete *_channels.begin();
    }
    setModified(false);
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "</MultichannelModel@" << this << "::clear>" << std::endl;
#endif
  }

  bool MultichannelModel::modified() const {
    bool modified = _modified;
    for (const_iterator it = begin(); it != end() && !modified; ++it)
      modified |= (*it)->modified();
    return modified;
  }

  void MultichannelModel::save(BlitzH5File &outFile, ProgressReporter *pr) {
    int pMin = 0, pMax = 100;
    if (pr != NULL) {
      pMin = pr->taskProgressMin();
      pMax = pr->taskProgressMax();
      pr->updateProgress(pMin);
    }
    if (outFile.existsGroup(".labelling_qt4"))
      outFile.deleteGroup(".labelling_qt4");
    outFile.writeAttribute(_channels.size(), "nChannels", "/.labelling_qt4");
    float pScale = static_cast<float>(pMax - pMin) /
      static_cast<float>(_channels.size());
    int i = 0;
    for (const_iterator it = begin(); it != end(); ++it, ++i) {
      if (pr != NULL) {
        if (!pr->updateProgress(pMin + static_cast<int>(i * pScale))) return;
        pr->setTaskProgressMin(pMin + static_cast<int>(i * pScale));
        pr->setTaskProgressMax(pMin + static_cast<int>((i + 1) * pScale));
      }
      (*it)->save(outFile, i, pr);
    }
    if (pr != NULL) {
      pr->setTaskProgressMin(pMin);
      pr->setTaskProgressMax(pMax);
    }
    setModified(false);
  }

  void MultichannelModel::load(BlitzH5File &inFile, ProgressReporter *pr) {
    clear();

    int pMin = 0, pMax = 100;
    if (pr != NULL) {
      pMin = pr->taskProgressMin();
      pMax = pr->taskProgressMax();
      pr->updateProgress(pMin);
    }
    size_t nChannels;
    inFile.readAttribute(nChannels, "nChannels", "/.labelling_qt4");
    float pScale = static_cast<float>(pMax - pMin) /
      static_cast<float>(nChannels);
    for (size_t i = 0; i < nChannels; ++i) {
      if (pr != NULL) {
        if (!pr->updateProgress(pMin + static_cast<int>(i * pScale))) return;
        pr->setTaskProgressMin(pMin + static_cast<int>(i * pScale));
        pr->setTaskProgressMax(pMin + static_cast<int>((i + 1) * pScale));
      }
      addChannel(Channel::createFromFile(inFile, i, pr));
    }
    if (pr != NULL) {
      pr->setTaskProgressMin(pMin);
      pr->setTaskProgressMax(pMax);
    }
    setModified(false);
  }

  void MultichannelModel::setModified(bool modified) {
    if (_modified == modified) return;
    _modified = modified;
    for (std::set<MultichannelModelObserver*>::const_iterator it =
      _observers.begin(); it != _observers.end(); ++it)
      (*it)->updateModified(modified);
  }

  std::string MultichannelModel::ensureValidName(Channel const *channel) const {
    if (channel == NULL) return "";
    std::vector<std::string> nameComponents(
      Channel::nameComponents(channel->name()));
    std::vector< std::vector<std::string> > existingComponents;
    for (const_iterator it = begin(); it != end(); ++it)
      if (*it != channel)
        existingComponents.push_back((*it)->nameComponents());

    // Current name string is empty. Create a new name
    if (nameComponents.size() == 0) {
      int index = 0;
      bool unique = false;
      std::string name = "";
      while (!unique) {
        std::stringstream nameStream;
        nameStream << "channel" << index;
        name = nameStream.str();
        unique = true;
        for (size_t j = 0; j < existingComponents.size() && unique; ++j)
          unique &= (name != existingComponents[j][0]);
        ++index;
      }
      return "/" + name;
    }

    bool collides = true;
    while (collides) {
      collides = false;
      for (size_t j = 0; j < existingComponents.size(); ++j) {
        if (nameComponents.size() < existingComponents[j].size()) {
          size_t i = 0;
          for (; i < nameComponents.size(); ++i)
            if (nameComponents[i] != existingComponents[j][i]) break;
          if (i == nameComponents.size()) {
            collides = true;
            nameComponents.back() += "_";
            break;
          }
        }
        else {
          size_t i = 0;
          for (; i < existingComponents[j].size(); ++i)
            if (nameComponents[i] != existingComponents[j][i]) break;
          if (i == existingComponents[j].size()) {
            collides = true;
            nameComponents[i - 1] += "_";
            break;
          }
        }
      }
    }
    std::string out = "";
    for (size_t i = 0; i < nameComponents.size(); ++i)
      out += "/" + nameComponents[i];
    return out;
  }

}
