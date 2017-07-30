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

#ifndef IROCS_CHANNELTREEWIDGETITEM_HH
#define IROCS_CHANNELTREEWIDGETITEM_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QTreeWidgetItem>
#include <libMultiChannelModel/ChannelObserver.hh>

namespace iRoCS {

  class MultichannelModelTreeWidget;
  class Channel;

  class ChannelTreeWidgetItem : public QTreeWidgetItem, public ChannelObserver {

  protected:

    ChannelTreeWidgetItem(
      Channel &channel, MultichannelModelTreeWidget *treeWidget = NULL);

  public:

    virtual ~ChannelTreeWidgetItem();

    /*======================================================================*/
    /*!
     *   Get the absolute channel name of the given ChannelTreeWidgetItem
     *   including all parent groups in the form '[/<ancestor>*]/<parent>/<name>'
     *
     *   \return The full absolute path of the channel as it can be stored in
     *     the channel's name attribute
     */
     /*======================================================================*/
    std::string absoluteName() const;

    /*======================================================================*/
    /*!
     *   Changes the entry of the path column (0) to reflect the channel path.
     *   This might require a reorganization of the tree.
     */
     /*======================================================================*/
    virtual void updateName();

    /*======================================================================*/
    /*!
     *   This method is called when the filename of the observed Channel changes.
     *   Corresponding views should be updated accordingly.
     */
     /*======================================================================*/
    virtual void updateFilename();

    /*======================================================================*/
    /*!
     *   Adjusts the alpha control to the channels alpha value
     */
     /*======================================================================*/
    virtual void updateAlpha();

    /*======================================================================*/
    /*!
     *   Checks/Unchecks the visible checkbox of the item
     */
     /*======================================================================*/
    virtual void updateVisibilityFlag();

    /*======================================================================*/
    /*!
     *   Marks the channel as modified/unmodified
     */
     /*======================================================================*/
    virtual void updateModifiedFlag();

    /*======================================================================*/
    /*!
     *   Factory method to produce a new ChannelTreeWidgetItem of type
     *   corresponding to the given channel type.
     *
     *   \param type The channel type to create a new ChannelTreeWidgetItem for
     *
     *   \return A pointer to the new ChannelTreeWidgetItem
     */
     /*======================================================================*/
    static ChannelTreeWidgetItem *instance(
      Channel &channel, MultichannelModelTreeWidget *treeWidget = NULL);

  private:

    bool _nameUpdateInProgress;

  };

}

#endif
