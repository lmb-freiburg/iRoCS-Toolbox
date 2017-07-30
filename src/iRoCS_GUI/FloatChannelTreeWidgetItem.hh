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

#ifndef IROCS_FLOATCHANNELTREEWIDGETITEM_HH
#define IROCS_FLOATCHANNELTREEWIDGETITEM_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "ChannelTreeWidgetItem.hh"

namespace iRoCS {

  class FloatChannel;

  class FloatChannelTreeWidgetItem : public ChannelTreeWidgetItem {

    FloatChannelTreeWidgetItem(
      FloatChannel &channel, MultichannelModelTreeWidget *treeWidget = NULL);

  public:

    ~FloatChannelTreeWidgetItem();

    /*======================================================================*/
    /*!
     *   This method is called when the voxel extents in micrometers of the
     *   observed Channel change. This method triggers OrthoViewWidget::update().
     */
     /*======================================================================*/
    void updateElementSizeUm();

    /*======================================================================*/
    /*!
     *   This method is called when the homogeneous backwarp transformation of
     *   the observed Channel changes. It requests a full update of the parent
     *   orthoview.
     */
     /*======================================================================*/
    void updateTransformation();

    /*======================================================================*/
    /*!
     *   This method is called when the data Array of the observed FloatChannel
     *   changes. Corresponding views should be updated accordingly.
     */
     /*======================================================================*/
    void updateData();

    /*======================================================================*/
    /*!
     *   Create a new FloatChannelTreeWidgetItem for the given
     *   FloatChannel/MulitchannelModelTreeWidget combination if it
     *   does not exist yet. If it already exists get the pointer to the
     *   existing instance.
     *
     *   \param channel The FloatChannel to create a FloatChannelTreeWidgetItem
     *     for
     *   \param treeWidget The MultichannelModelTreeWidget to create
     *     a FloatChannelTreeWidgetItem for
     *
     *   \return The singleton FloatChannelTreeWidgetItem
     */
     /*======================================================================*/
    static FloatChannelTreeWidgetItem *instance(
      FloatChannel &channel, MultichannelModelTreeWidget *treeWidget = NULL);

  };

}

#endif
