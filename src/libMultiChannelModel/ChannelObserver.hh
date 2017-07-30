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

#ifndef IROCS_CHANNELOBSERVER_HH
#define IROCS_CHANNELOBSERVER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

namespace iRoCS {

  class Channel;
  class Observable;

  /*======================================================================*/
  /*!
   *  \class ChannelObserver ChannelObserver.hh <liblabelling_qt4/ChannelObserver.hh>
   *  \brief The ChannelObserver class specifies the interface
   *    that must be implemented by classes that shall be notified at channel
   *    changes.
   *
   *  This class interfaces the pure C++ Channel class to QT
   *  view- and control widgets. The class is pure virtual, therefore it
   *  must be sub-classed and corresponding functionality implemented.
   */
   /*======================================================================*/
  class ChannelObserver {

  public:

    /*======================================================================*/
    /*!
     *   Create a new ChannelObserver for the given Channel.
     *
     *   \param channel The observed Channel
     */
     /*======================================================================*/
    ChannelObserver(Channel &channel);

    /*======================================================================*/
    /*!
     *   Destroy this ChannelObserver.
     *
     *   If the observer is registered to a Channel at time of destruction,
     *   it will be unregistered from the Channel first.
     */
     /*======================================================================*/
    virtual ~ChannelObserver();

    /*======================================================================*/
    /*!
     *   Get the Channel this ChannelObserver observes.
     *
     *   \return A read only reference to the observed Channel
     */
     /*======================================================================*/
    Channel const &channel() const;

    /*======================================================================*/
    /*!
     *   Get the Channel this ChannelObserver observes.
     *
     *   \return A random access reference to the observed Channel
     */
     /*======================================================================*/
    Channel &channel();

    /*======================================================================*/
    /*!
     *   This method is called when the name of the observed Channel changes.
     *   Corresponding views should be updated accordingly.
     */
     /*======================================================================*/
    virtual void updateName() = 0;

    /*======================================================================*/
    /*!
     *   This method is called when the filename of the observed Channel changes.
     *   Corresponding views should be updated accordingly.
     */
     /*======================================================================*/
    virtual void updateFilename() = 0;

    /*======================================================================*/
    /*!
     *   This method is called when the transparency of the observed Channel
     *   changes. Corresponding views should be updated accordingly.
     */
     /*======================================================================*/
    virtual void updateAlpha() = 0;

    /*======================================================================*/
    /*!
     *   This method is called when the visibility of the observed Channel
     *   changes. Corresponding views should be updated accordingly.
     */
     /*======================================================================*/
    virtual void updateVisibilityFlag() = 0;

    /*======================================================================*/
    /*!
     *   This method is called when the modified flag of the observed Channel
     *   changes. Corresponding views should be updated accordingly.
     */
     /*======================================================================*/
    virtual void updateModifiedFlag() = 0;

    /*======================================================================*/
    /*!
     *   This method is called when the voxel extents in micrometers of the
     *   observed Channel change. Corresponding views should be updated
     *   accordingly.
     */
     /*======================================================================*/
    virtual void updateElementSizeUm() = 0;

    /*======================================================================*/
    /*!
     *   This method is called when the homogeneous backwarp transformation of
     *   the observed Channel changes. Corresponding views should be updated
     *   accordingly.
     */
     /*======================================================================*/
    virtual void updateTransformation() = 0;

  protected:

    Channel &_channel;

  };

}

#endif
