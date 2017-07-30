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

#include "MultichannelModelTreeWidget.hh"

#include <QtGui/QMenu>
#include <QtGui/QDropEvent>

#include <libMultiChannelModel/MultichannelModelObserver.hh>
#include <libMultiChannelModel/MultichannelModel.hh>
#include "ChannelTreeWidgetItem.hh"
#include "ChannelControlWidget.hh"
#include "LineEditDelegate.hh"
#include "DoubleSpinBoxDelegate.hh"

namespace iRoCS {

  MultichannelModelTreeWidget::MultichannelModelTreeWidget(
    MultichannelModel &model, QWidget *parent)
    : QTreeWidget(parent), MultichannelModelObserver(model) {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<MultichannelModelTreeWidget@" << this
      << "::MultichannelModelTreeWidget model=\"" << &model
      << "\" parent=\"" << parent << "\">" << std::endl;
#endif
    setColumnCount(3);
    QStringList headerLabels;
    headerLabels << tr("Path") << tr("alpha") << tr("Channel type");
    setHeaderLabels(headerLabels);
    setDragDropMode(QAbstractItemView::InternalMove);
    setDropIndicatorShown(true);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSortingEnabled(true);
    sortItems(0, Qt::AscendingOrder);
    invisibleRootItem()->setFlags(Qt::ItemIsEnabled);
    setItemDelegateForColumn(0, new LineEditDelegate(this));
    setItemDelegateForColumn(1, new DoubleSpinBoxDelegate(this));
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(itemChanged(QTreeWidgetItem *, int)),
      SLOT(updateModel(QTreeWidgetItem *, int)));
    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
      SLOT(handleEditRequest(QTreeWidgetItem *, int)));
    connect(this, SIGNAL(customContextMenuRequested(QPoint const &)),
      SLOT(handleCustomContextMenuRequest(QPoint const &)));
    updateModel();
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "</MultichannelModelTreeWidget@" << this
      << "::MultichannelModelTreeWidget>" << std::endl;
#endif
  }

  MultichannelModelTreeWidget::~MultichannelModelTreeWidget() {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<MultichannelModelTreeWidget@" << this
      << "::~MultichannelModelTreeWidget />" << std::endl;
#endif
  }

  void MultichannelModelTreeWidget::updateAddChannel(Channel *channel) {
    if (channel == NULL) return;

    // Search channel observer in tree widget
    QTreeWidgetItemIterator it(this, QTreeWidgetItemIterator::NoChildren);
    while (*it) {
      if ((*it)->type() > QTreeWidgetItem::UserType &&
        &static_cast<ChannelTreeWidgetItem*>(*it)->channel() == channel)
        break;
      ++it;
    }
    if (*it) return; // ChannelTreeWidgetItem already in tree

#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<MultichannelModelTreeWidget@" << this
      << "::updateAddChannel channel=\"" << channel << "\">"
      << std::endl;
#endif
    clearSelection();
    bool initiallyEmpty = (topLevelItemCount() == 0);
    ChannelTreeWidgetItem::instance(*channel, this);
    adjustColumnWidths();
    if (initiallyEmpty && topLevelItemCount() != 0)
      emit isEmptyChanged(false);
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "</MultichannelModelTreeWidget@" << this
      << "::updateAddChannel>" << std::endl;
#endif
  }

  void MultichannelModelTreeWidget::updateRemoveChannel(Channel *channel) {
    if (channel == NULL) return;

    // Search channel observer in tree widget
    QTreeWidgetItemIterator it(this, QTreeWidgetItemIterator::NoChildren);
    while (*it) {
      if ((*it)->type() > QTreeWidgetItem::UserType &&
        &static_cast<ChannelTreeWidgetItem*>(*it)->channel() == channel)
        break;
      ++it;
    }
    if (!*it) return; // ChannelTreeWidgetItem for channel not found

    QTreeWidgetItem *currentItem = *it;
    QTreeWidgetItem *currentGroup = currentItem->parent();
    if (currentGroup != NULL) currentGroup->removeChild(currentItem);
    delete currentItem;
    while (currentGroup != NULL &&
      currentGroup->childCount() == 0) {
      QTreeWidgetItem *parent = currentGroup->parent();
      if (parent != NULL) parent->removeChild(currentGroup);
      delete currentGroup;
      currentGroup = parent;
    }
    if (topLevelItemCount() == 0) emit isEmptyChanged(true);
  }

  void MultichannelModelTreeWidget::updateModified(bool modified) {
    emit modifiedFlagChanged(modified);
  }

  void MultichannelModelTreeWidget::updateModel() {
    if (topLevelItemCount() != 0) clear();
    for (MultichannelModel::const_iterator it = _model.begin();
      it != _model.end(); ++it) ChannelTreeWidgetItem::instance(**it, this);
  }

  void MultichannelModelTreeWidget::deleteSelectedChannels() {
    QList<QTreeWidgetItem*> selected = selectedItems();
    for (QList<QTreeWidgetItem*>::iterator it = selected.begin();
      it != selected.end(); ++it) {
      if ((*it)->type() > QTreeWidgetItem::UserType) {
        delete &static_cast<ChannelTreeWidgetItem*>(*it)->channel();
      }
    }
  }

  void MultichannelModelTreeWidget::updateModel(
    QTreeWidgetItem *item, int column) {
    if (item->type() <= QTreeWidgetItem::UserType) return;
    Channel &c = static_cast<ChannelTreeWidgetItem*>(item)->channel();
    switch (column) {
      case 0:
        c.setVisible(item->checkState(0) == Qt::Checked);
        c.setName(static_cast<ChannelTreeWidgetItem*>(item)->absoluteName());
        break;
      case 1:
        c.setAlpha(item->data(1, Qt::EditRole).toDouble());
        break;
    }
  }

  void MultichannelModelTreeWidget::handleEditRequest(
    QTreeWidgetItem *item, int column) {
    if (column < 2) editItem(item, column);
  }

  void MultichannelModelTreeWidget::adjustColumnWidths() {
    for (int c = 0; c < columnCount(); ++c) resizeColumnToContents(c);
  }

  void MultichannelModelTreeWidget::handleCustomContextMenuRequest(
    QPoint const &pos) {
    QTreeWidgetItem *item = itemAt(pos);
    if (item == NULL || item->type() <= QTreeWidgetItem::UserType) return;
    QMenu *menu = new QMenu;
    QAction *showPropertiesAction =
      menu->addAction(tr("Channel properties..."));
    QAction *deleteSelectedChannelsAction =
      menu->addAction(tr("Delete selected channels"));
    QAction *selectedAction = menu->exec(QCursor::pos());
    if (selectedAction == showPropertiesAction) showChannelProperties(
      &static_cast<ChannelTreeWidgetItem*>(item)->channel());
    else if (selectedAction == deleteSelectedChannelsAction)
      deleteSelectedChannels();
    delete menu;
  }

  void MultichannelModelTreeWidget::showChannelProperties(Channel *channel) {
    ChannelControlWidget::instance(*channel, NULL, true)->setVisible(true);
  }

  void MultichannelModelTreeWidget::dropEvent(QDropEvent *e) {
    e->ignore();

    // Get dropped items
    QList<QTreeWidgetItem*> items = selectedItems();

    // Get involved parent items to remove empty groups
    QSet<QTreeWidgetItem *> parents;
    for (QList<QTreeWidgetItem *>::iterator it = items.begin();
      it != items.end(); ++it) parents.insert((*it)->parent());

    // Call parent drop event to actually do the drop
    QTreeWidget::dropEvent(e);

    // Delete empty groups
    while (parents.size() != 0) {
      QSet<QTreeWidgetItem*>::iterator currentIter = parents.begin();
      QTreeWidgetItem *currentItem = *currentIter;
      if (currentItem->childCount() == 0) {
        QTreeWidgetItem *parentItem = currentItem->parent();
        delete currentItem;
        parents.erase(currentIter);
        parents.insert(parentItem);
      }
      else parents.erase(currentIter);
    }

    // Update moved channel names
    for (QList<QTreeWidgetItem*>::iterator it = items.begin();
      it != items.end(); ++it) {
      if ((*it)->type() > QTreeWidgetItem::UserType) {
        ChannelTreeWidgetItem *channelItem =
          static_cast<ChannelTreeWidgetItem*>(*it);
        channelItem->channel().setName(channelItem->absoluteName());
      }
    }

    // Restore old selection
    for (QList<QTreeWidgetItem *>::iterator it = items.begin();
      it != items.end(); ++it) (*it)->setSelected(true);
  }

  void MultichannelModelTreeWidget::keyReleaseEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Delete) {
      deleteSelectedChannels();
      e->accept();
      return;
    }

    QTreeWidget::keyReleaseEvent(e);
  }

}
