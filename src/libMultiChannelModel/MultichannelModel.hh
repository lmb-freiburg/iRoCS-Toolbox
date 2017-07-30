/**************************************************************************
 *
 * Copyright (C) 2016 Thorsten Falk
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

#ifndef IROCS_MULTICHANNELMODEL_HH
#define IROCS_MULTICHANNELMODEL_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <cstddef>
#include <set>

#include "Observable.hh"
#include "Channel.hh"

namespace iRoCS {

  class MultichannelModelObserver;

  /*======================================================================*/
  /*!
   *  \class MultichannelModel MultichannelModel.hh <liblabelling_qt4/MultichannelModel.hh>
   *  \brief The MultichannelModel class combines individual Channel objects
   *    in a model that can be viewed, edited, loaded and saved to hdf5.
   */
   /*======================================================================*/
  class MultichannelModel : public Observable {

#ifdef DEBUG_VERBOSE_XML
    std::string printState() const;
#endif

  public:

    /*======================================================================*/
    /*!
     *  The datatype for a random access iterator through the channels of
     *  this model
     */
     /*======================================================================*/
    typedef std::set<Channel*>::iterator iterator;

    /*======================================================================*/
    /*!
     *  The datatype for a read-only iterator through the channels of this model
     */
     /*======================================================================*/
    typedef std::set<Channel*>::const_iterator const_iterator;

    /*======================================================================*/
    /*!
     *   Creates a new empty MultichannelModel object.
     */
     /*======================================================================*/
    MultichannelModel();

    /*======================================================================*/
    /*!
     *   Deletes this MultichannelModel object and all channels it owns. Cleanup
     *   order: remove all channels from this model (Observers are still
     *   notified) and delete them, then all MultichannelModelObservers refering
     *   to this MultichannelModel will be deleted.
     */
     /*======================================================================*/
    ~MultichannelModel();

    /*======================================================================*/
    /*!
     *   Add (append) the given channel to this model. The MultichannelModel
     *   takes ownership of the given Channel. If you want to keep the Channel,
     *   make sure to remove it from this MultichannelModel before deleting the
     *   model.
     *
     *   MultichannelModelObserver instances registered to this MultichannelModel
     *   will be notified via MultichannelModelObserver::addChannel().
     *
     *   \param channel A pointer to the Channel object to add
     */
     /*======================================================================*/
    void addChannel(Channel *channel);

    /*======================================================================*/
    /*!
     *   Remove the given channel from this model. Channel data is not deleted
     *   using this method, so the Channel pointer remains valid only its parent
     *   will be set to NULL.
     *
     *   MultichannelModelObserver instances registered to this MultichannelModel
     *   will be notified via MultichannelModelObserver::removeChannel().
     *
     *   \param channel A pointer to the Channel object to add
     */
     /*======================================================================*/
    void removeChannel(Channel *channel);

    /*======================================================================*/
    /*!
     *   Get the number of channels of specified types in this MultichannelModel.
     *
     *   \param types A list of ORed Channel::Type enumeration values.
     *     If set to any the overall number of channels is returned
     *
     *   \return The overall number of channels of any specified type
     */
     /*======================================================================*/
    size_t nChannels(Channel::Types types = Channel::Any) const;

    /*======================================================================*/
    /*!
     *   Get a read-only iterator to the beginning of the channel vector.
     *
     *   \return An iterator to the first entry of the channel vector. If the
     *     model contains no channels begin() == end()
     */
     /*======================================================================*/
    const_iterator begin() const;

    /*======================================================================*/
    /*!
     *   Get a random access iterator to the beginning of the channel vector.
     *
     *   \return An iterator to the first entry of the channel vector. If the
     *     model contains no channels begin() == end()
     */
     /*======================================================================*/
    iterator begin();

    /*======================================================================*/
    /*!
     *   Get a read-only iterator beyond the end of the channel vector.
     *
     *   \return An iterator to one entry behind the last of the channel vector.
     *     If the model contains no channels begin() == end()
     */
     /*======================================================================*/
    const_iterator end() const;

    /*======================================================================*/
    /*!
     *   Get a random access iterator beyond the end of the channel vector.
     *
     *   \return An iterator to one entry behind the last of the channel vector.
     *     If the model contains no channels begin() == end()
     */
     /*======================================================================*/
    iterator end();

    /*======================================================================*/
    /*!
     *   Remove and delete all channels from this MultichannelModel. If you
     *   want to keep the Channel data use removeChannel() for the individual
     *   channels instead.
     */
     /*======================================================================*/
    void clear();

    /*======================================================================*/
    /*!
     *   Set the modified flag for this model. The modified flag is mainly
     *   used for enabling and disabling Save actions and for visualizing this
     *   state. Besides the modified flag of the model, each channel contained
     *   in the model provide their own modified flags to report modifications
     *   that need to be saved. Clearing this flag (setting it to false) does
     *   not change the modified flags of any channel within the model.
     *
     *   \param modified The new modified flag.
     */
     /*======================================================================*/
    void setModified(bool modified);

    /*======================================================================*/
    /*!
     *   Check whether this model has modified data.
     *
     *   \return true if this model is itself modified by added or removed
     *     channels or changed channel meta data or if it contains at least
     *     one channel with set modified flag.
     */
     /*======================================================================*/
    bool modified() const;

    /*======================================================================*/
    /*!
     *   Save this MultichannelModel to the specified hdf5 file. Additionally
     *   to the channels and their group structure a group /.labelling_qt4 is
     *   created that stores the visualization meta data of the channels.
     *   If that group already exists, it will be replaced. If channel structures
     *   and already existing hdf5 content are disjoint the file will only be
     *   extended otherwise file content is overwritten if possible.
     *
     *   \param outFile The hdf5 file to save the model to
     *
     *   \exception BlitzH5Error If the model cannot be saved this error is
     *     thrown
     */
     /*======================================================================*/
    void save(BlitzH5File &outFile, ProgressReporter *pr = NULL);

    /*======================================================================*/
    /*!
     *   Load the MultichannelModel stored in the given hdf5 iRoCS Toolbox
     *   project file. The model is cleared before loading.
     *
     *   \param inFile The hdf5 file to load the model from
     *
     *   \exception BlitzH5Error If the model cannot be loaded this error is
     *     thrown
     */
     /*======================================================================*/
    void load(BlitzH5File &inFile, ProgressReporter *pr = NULL);

    /*======================================================================*/
    /*!
     *   This function checks whether the name of the given channel can be
     *   savely used in this model. It checks whether the name is a valid
     *   hdf5 path descriptor and whether it collides with existing channels.
     *   If it collides, an alternative channel name string is returned that
     *   can be used in the model.
     *
     *   \param channel The channel for which to find a name that allows unique
     *     identification of the channel in this model
     *
     *   \return A unique channel name with respect to this model
     */
     /*======================================================================*/
    std::string ensureValidName(Channel const *channel) const;

  private:

    std::set<MultichannelModelObserver*> _observers;
    std::set<Channel*> _channels;
    bool _modified;

    // Make the MultichannelModelObserver class a friend, because it
    // may register and unregister itself by directly modifying the
    // _observers set
    friend class MultichannelModelObserver;

  };

}

#endif
