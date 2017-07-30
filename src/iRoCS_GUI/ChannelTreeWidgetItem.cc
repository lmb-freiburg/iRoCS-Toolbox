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

#include <libMultiChannelModel/FloatChannel.hh>
#include "FloatChannelTreeWidgetItem.hh"
 // #include "IndexedChannelTreeWidgetItem.hh"
 // #include "RGBChannelTreeWidgetItem.hh"
 // #include "AnnotationChannelTreeWidgetItem.hh"
#include <libMultiChannelModel/MultichannelModel.hh>
#include "MultichannelModelTreeWidget.hh"
#include <libMultiChannelModel/Channel.hh>

#include <stdexcept>

namespace iRoCS {

  ChannelTreeWidgetItem::ChannelTreeWidgetItem(
    Channel &channel, MultichannelModelTreeWidget *treeWidget)
    : QTreeWidgetItem(QTreeWidgetItem::UserType + channel.type()),
    ChannelObserver(channel), _nameUpdateInProgress(false) {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<ChannelTreeWidgetItem@" << this
      << "::ChannelTreeWidgetItem channel=\"" << &channel
      << "\" treeWidget=\"" << treeWidget << "\">" << std::endl;
#endif
    if (treeWidget != NULL) {
      QTreeWidgetItem *currentItem = NULL;
      if (treeWidget->topLevelItemCount() == 0) {
        QStringList rootItemLabels;
        rootItemLabels << "/" << "" << "";
        QTreeWidgetItem *rootItem = new QTreeWidgetItem(
          rootItemLabels, QTreeWidgetItem::UserType);
        rootItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsDropEnabled);
        treeWidget->addTopLevelItem(rootItem);
        currentItem = rootItem;
      }
      else currentItem = treeWidget->topLevelItem(0);

      // Extract group structure from channel name string
      std::vector<std::string> pathComponents(_channel.nameComponents());
      if (pathComponents.size() == 0) {
        if (_channel.model() != NULL)
          _channel.setName(_channel.model()->ensureValidName(&_channel));
        else _channel.setName("channel");
        pathComponents = _channel.nameComponents();
      }

      for (size_t i = 0; i < pathComponents.size() - 1; ++i) {
        int j = 0;
        for (; j < currentItem->childCount(); ++j)
          if (currentItem->child(j)->text(0) == pathComponents[i].c_str())
            break;
        if (j < currentItem->childCount()) {
          if (currentItem->child(j)->type() != QTreeWidgetItem::UserType) {
            // ToDo: React on the problem at best with user interaction
            std::cerr << "Cannot add group " << pathComponents[i]
              << " a channel with that name already exists"
              << std::endl;
#ifdef DEBUG_VERBOSE_XML
            std::cerr << "</ChannelTreeWidgetItem@" << this
              << "::ChannelTreeWidgetItem>" << std::endl;
#endif
            return;
          }
          currentItem = currentItem->child(j);
        }
        else {
          QStringList itemLabels;
          itemLabels << pathComponents[i].c_str() << "" << "";
          QTreeWidgetItem *item = new QTreeWidgetItem(
            itemLabels, QTreeWidgetItem::UserType);
          currentItem->addChild(item);
          item->setFlags(
            Qt::ItemIsEnabled | Qt::ItemIsDragEnabled |
            Qt::ItemIsDropEnabled);
          currentItem->setExpanded(true);
          currentItem = item;
        }
      }

      // Create leaf for channel
      int j = 0;
      for (; j < currentItem->childCount(); ++j)
        if (currentItem->child(j)->text(0) == pathComponents.back().c_str())
          break;
      if (j < currentItem->childCount()) {
        // ToDo: React on the problem at best with user interaction
        std::cerr << "Cannot add channel " << channel.name()
          << " a channel or group with that name already exists"
          << std::endl;
#ifdef DEBUG_VERBOSE_XML
        std::cerr << "</ChannelTreeWidgetItem@" << this
          << "::ChannelTreeWidgetItem>" << std::endl;
#endif
        return;
      }
      setText(0, pathComponents.back().c_str());
      setCheckState(0, _channel.visible() ? Qt::Checked : Qt::Unchecked);
      setText(1, QString::number(_channel.alpha()));
      currentItem->addChild(this);
      currentItem->setExpanded(true);
      setFlags(Qt::ItemIsEnabled | Qt::ItemIsDragEnabled |
        Qt::ItemIsUserCheckable | Qt::ItemIsEditable |
        Qt::ItemIsSelectable);
      setSelected(true);
    }
    else {
      setText(0, _channel.name().c_str());
      setCheckState(0, _channel.visible() ? Qt::Checked : Qt::Unchecked);
      setText(1, QString::number(_channel.alpha()));
      setFlags(Qt::ItemIsEnabled | Qt::ItemIsDragEnabled |
        Qt::ItemIsUserCheckable | Qt::ItemIsEditable |
        Qt::ItemIsSelectable);
    }

#ifdef DEBUG_VERBOSE_XML
    std::cerr << "</ChannelTreeWidgetItem@" << this
      << "::ChannelTreeWidgetItem>" << std::endl;
#endif
  }

  ChannelTreeWidgetItem::~ChannelTreeWidgetItem() {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<ChannelTreeWidgetItem@" << this
      << "::~ChannelTreeWidgetItem />" << std::endl;
#endif
  }

  std::string ChannelTreeWidgetItem::absoluteName() const {
    if (treeWidget() == NULL || parent() == NULL) return text(0).toStdString();
    std::string name;
    QStringList pathComponents;
    QTreeWidgetItem const *currentItem = this;
    while (currentItem->text(0) != "/") {
      name = currentItem->text(0).toStdString() +
        ((name != "") ? "/" + name : "");
      currentItem = currentItem->parent();
    }
    return name;
  }

  void ChannelTreeWidgetItem::updateName() {
    if (_nameUpdateInProgress) return;
    _nameUpdateInProgress = true;
    if (treeWidget() == NULL) {
      setText(0, _channel.name().c_str());
      _nameUpdateInProgress = false;
      return;
    }

    std::vector<std::string> nameComponents(_channel.nameComponents());
    std::vector<std::string> oldNameComponents;
    QTreeWidgetItem *currentItem = this;
    while (currentItem != NULL && currentItem->text(0) != "/") {
      oldNameComponents.insert(
        oldNameComponents.begin(), currentItem->text(0).toStdString());
      currentItem = currentItem->parent();
    }

    size_t startGroupIndex = 0;
    QTreeWidgetItem *startGroupItem = treeWidget()->topLevelItem(0);
    for (; startGroupIndex < oldNameComponents.size() - 1 &&
      startGroupIndex < nameComponents.size() - 1; ++startGroupIndex) {
      if (oldNameComponents[startGroupIndex] != nameComponents[startGroupIndex])
        break;
      for (int j = 0; j < startGroupItem->childCount(); ++j) {
        if (startGroupItem->child(j)->text(0) ==
          nameComponents[startGroupIndex].c_str()) {
          startGroupItem = startGroupItem->child(j);
          break;
        }
      }
    }

    // The name did not actually change, nothing to do
    if (oldNameComponents.size() == nameComponents.size() &&
      startGroupIndex == nameComponents.size() - 1 &&
      oldNameComponents.back() == nameComponents.back()) {
      _nameUpdateInProgress = false;
      return;
    }

    // Simple renaming
    if (startGroupItem == parent() &&
      startGroupIndex == nameComponents.size() - 1) {
      setText(0, nameComponents.back().c_str());
      _nameUpdateInProgress = false;
      return;
    }

    QTreeWidgetItem *parentItem = parent();
    parentItem->takeChild(parentItem->indexOfChild(this));
    setText(0, nameComponents.back().c_str());

    // Recursively add groups till the channel item can be moved
    currentItem = startGroupItem;
    for (size_t i = startGroupIndex; i < nameComponents.size() - 1; ++i) {
      int j = 0;
      for (; j < currentItem->childCount(); ++j)
        if (currentItem->child(j)->text(0) == nameComponents[i].c_str())
          break;
      if (j < currentItem->childCount()) currentItem = currentItem->child(j);
      else {
        QStringList itemLabels;
        itemLabels << nameComponents[i].c_str() << "" << "";
        QTreeWidgetItem *item = new QTreeWidgetItem(itemLabels, 0);
        currentItem->addChild(item);
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsDropEnabled);
        item->setExpanded(true);
        currentItem = item;
      }
    }
    currentItem->addChild(this);

    // Recursively remove empty groups till startItem
    currentItem = parentItem;
    while (currentItem != startGroupItem && currentItem->childCount() == 0) {
      delete currentItem;
      currentItem = parentItem;
    }

    setSelected(true);
    static_cast<MultichannelModelTreeWidget*>(
      treeWidget())->adjustColumnWidths();

    _nameUpdateInProgress = false;
  }

  void ChannelTreeWidgetItem::updateFilename() {}

  void ChannelTreeWidgetItem::updateAlpha() {
    setText(1, QString::number(_channel.alpha()));
  }

  void ChannelTreeWidgetItem::updateVisibilityFlag() {
    setCheckState(0, _channel.visible() ? Qt::Checked : Qt::Unchecked);
  }

  void ChannelTreeWidgetItem::updateModifiedFlag() {}

  ChannelTreeWidgetItem *ChannelTreeWidgetItem::instance(
    Channel &channel, MultichannelModelTreeWidget *treeWidget) {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<ChannelTreeWidgetItem::instance channel=\"" << &channel
      << "\" treeWidget=\"" << treeWidget << "\">" << std::endl;
#endif
    try {
      FloatChannelTreeWidgetItem *item = FloatChannelTreeWidgetItem::instance(
        dynamic_cast<FloatChannel&>(channel), treeWidget);
#ifdef DEBUG_VERBOSE_XML
      std::cerr << "return " << item << std::endl;
      std::cerr << "</ChannelTreeWidgetItem::instance>" << std::endl;
#endif
      return item;
    }
    catch (std::bad_cast &) {
    }
    std::stringstream msg;
    msg << "Could not create a ChannelTreeWidgetItem for the given channel "
      << "with address " << &channel << ". The given Channel type has no "
      << "ChannelTreeWidgetItem implementation.";
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "throw std::runtime_error \"" << msg.str() << "\""
      << std::endl;
    std::cerr << "</ChannelTreeWidgetItem::instance>" << std::endl;
#endif
    throw std::runtime_error(msg.str().c_str());
  }

}
