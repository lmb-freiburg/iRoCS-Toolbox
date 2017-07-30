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

#ifndef IROCS_CHANNELORTHOVIEWRENDERER_HH
#define IROCS_CHANNELORTHOVIEWRENDERER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "OrthoviewRenderer.hh"
#include <libMultiChannelModel/ChannelObserver.hh>

namespace iRoCS {

  class Channel;

  class ChannelOrthoviewRenderer
    : public OrthoviewRenderer, public ChannelObserver {

  protected:

    ChannelOrthoviewRenderer(OrthoviewWidget &view, Channel &channel);

  public:

    virtual ~ChannelOrthoviewRenderer();

    /*======================================================================*/
    /*!
     *   Create a new concrete OrthoviewRenderer for the given
     *   Channel/OrthoviewWidget combination if it does not exist yet. If
     *   it already exists get the pointer to the existing instance.
     *
     *   \param view The OrthoviewWidget to create a renderer for
     *   \param channel The Channel to create a renderer for
     *
     *   \return The new ChannelOrthoviewRenderer
     *
     *   \exception std::runtime_error is thrown if no OrthoviewRenderer could
     *     be created. This may indicate missing implementation for the
     *     type of channel.
     */
     /*======================================================================*/
    static ChannelOrthoviewRenderer *instance(
      OrthoviewWidget &view, Channel &channel);

  };

}

#endif
