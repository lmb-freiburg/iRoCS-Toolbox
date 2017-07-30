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

#ifndef IROCS_FLOATCHANNELCONTROLWIDGET_HH
#define IROCS_FLOATCHANNELCONTROLWIDGET_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "ChannelControlWidget.hh"

namespace iRoCS {

  class FloatChannel;

  class FloatChannelControlWidget : public ChannelControlWidget {

    Q_OBJECT

      FloatChannelControlWidget(
        FloatChannel &channel,
        MultichannelModelControlWidgetStack *controlStack = NULL,
        bool advancedMode = false);

  public:

    ~FloatChannelControlWidget();

    /*======================================================================*/
    /*!
     *   This method is called when the data Array of the observed FloatChannel
     *   changes. This method triggers FloatChannelControlWidget::update().
     */
     /*======================================================================*/
    void updateData();

    /*======================================================================*/
    /*!
     *   Create a new FloatChannelControlWidget for the given
     *   FloatChannel/MultichannelModelControlWidgetStack combination if it
     *   does not exist yet. If it already exists get the pointer to the
     *   existing instance.
     *
     *   \param channel The FloatChannel to create a FloatChannelControlWidget for
     *   \param controlStack The MultichannelModelControlWidgetStack to create
     *     a FloatChannelControlWidget for
     *
     *   \return The new FloatChannelControlWidget
     */
     /*======================================================================*/
    static FloatChannelControlWidget *instance(
      FloatChannel &channel,
      MultichannelModelControlWidgetStack *controlStack = NULL,
      bool advancedMode = false);

    private slots:

    void normalizeIndexRange();

  };

}

#endif
