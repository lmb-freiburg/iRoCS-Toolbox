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

/*======================================================================*/
/*!
 *  \file MultiChannelModel.hh
 *  \brief MultiChannelModel combines a vector of Channel objects into
 *    a common model.
 */
/*======================================================================*/

#ifndef ATBMULTICHANNELMODEL_HH
#define ATBMULTICHANNELMODEL_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "Array.hh"
#include "../ATBSpline.hh"
#include "../iRoCS.hh"

// #include "Marker.hh"
#include "Channel.hh"

namespace atb
{
  
  class FloatChannel;
  // class IntegerChannel;
  // class RGBChannel;
  // class AnnotationChannel;
  // class SplineCurveChannel;
  // class IRoCSChannel;
  
  class MultiChannelModelObserver;

/*======================================================================*/
/*!
 *  \class MultiChannelModel MultiChannelModel.hh "atb/MultiChannelModel.hh"
 *  \brief The MultiChannelModel class provides a unified way of handling
 *    a vector of Channel objects.
 *
 *  This class belongs to the iRoCS Toolbox framework and is the internal
 *  representation of the iRoCS Toolbox workspace, consisting of a set
 *  of channels with different properties. All channels share a common
 *  coordinate system and element size.
 */
/*======================================================================*/
  class MultiChannelModel
  {
    
  public:
    
/*======================================================================*/
/*! 
 *   The read-write iterator type to iterate over the channels contained
 *   in a MultiChannelModel
 */
/*======================================================================*/
    typedef std::vector<ChannelSpecs*>::iterator iterator;

/*======================================================================*/
/*! 
 *   The read-only iterator type to iterate over the channels contained
 *   in a MultiChannelModel
 */
/*======================================================================*/
    typedef std::vector<ChannelSpecs*>::const_iterator const_iterator;
    
/*======================================================================*/
/*! 
 *   Constructor. Creates an empty MultiChannelModel. The shape in pixels
 *   is set to zero and the element size to one.
 */
/*======================================================================*/
    MultiChannelModel();

/*======================================================================*/
/*! 
 *   Destructor. Detaches all observers from this model and removes the
 *   associated channels if they are not associated to other models. 
 */
/*======================================================================*/
    ~MultiChannelModel();
    
/*======================================================================*/
/*! 
 *   Get the model shape in pixels.
 *
 *   \return The model shape in pixels
 */
/*======================================================================*/
    blitz::TinyVector<ptrdiff_t,3> const &shapePx() const;

/*======================================================================*/
/*! 
 *   Get the model element size in micrometers.
 *
 *   \return The model element size in micrometers
 */
/*======================================================================*/
    blitz::TinyVector<double,3> const &elementSizeUm() const;

/*======================================================================*/
/*! 
 *   Set the active channel for user interaction. This function is intended
 *   for interactive model editing. After successful change all observers
 *   are notifed by calling their activeChannelChangeEvent() method. If the
 *   channel cannot be activated (because it is not within this model), this
 *   function is a noop. 
 *
 *   \param index The index of the channel to activate
 */
/*======================================================================*/
    void setActiveChannel(int index);

/*======================================================================*/
/*! 
 *   Get the index of the active channel. This function
 *   is intended for interactive model editing. 
 *
 *   \return The index of the active Channel
 */
/*======================================================================*/
    int activeChannel() const;

/*======================================================================*/
/*! 
 *   Set the modified flag of this MultiChannelModel.
 *
 *   \param modified The new modified flag value
 */
/*======================================================================*/
    void setModified(bool modified);

/*======================================================================*/
/*! 
 *   Get the modified flag of this MultiChannelModel.
 *
 *   \return A flag indicating whether the model has possibly changed
 *     since the last time this flag has been cleared
 */
/*======================================================================*/
    bool modified() const;

/*======================================================================*/
/*! 
 *   Create a new FloatChannel object from the given data Array and add it to
 *   this MultiChannelModel. If the Array shape or element size are
 *   incompatible, the function returns a NULL pointer indicating, that
 *   the operation failed. Upon successfull channel addition, all
 *   observers are notified by calling their channelAddEvent() method.
 *   If the NULL pointer is passed instead of a pre-existing Array a new
 *   Array will be created based on the shape and element size information
 *   of the model. This is only possible if the model already has a valid
 *   shape and element size. The ownership of the given Array is passed to
 *   the created channel. If the new channel is the only channel in the
 *   model it is activated.
 *   On success this automatically sets the modified flag of the model.
 *
 *   <b>Warning:</b> If the Array is compatible, but for any unknown
 *   reason the created FloatChannel cannot be added to the MultiChannelModel,
 *   then the Array channelData was pointing to are deleted and the pointer
 *   set to the NULL pointer. If this happens it is a bug!
 *
 *   \param channelData A pointer to the float Array containing the
 *     channel data or the NULL pointer for on-the-fly generation
 *
 *   \return A pointer to the newly created channel or the NULL pointer
 *     if the creation failed
 */
/*======================================================================*/
    // FloatChannel *addFloatChannel(Array<float,3> *channelData = NULL);

/*======================================================================*/
/*! 
 *   Create a new IntegerChannel object from the given data Array and add
 *   it to this MultiChannelModel. If the Array shape or element size are
 *   incompatible, the function returns a NULL pointer indicating, that
 *   the operation failed. Upon successfull channel addition, all
 *   observers are notified by calling their channelAddEvent() method.
 *   If the NULL pointer is passed instead of a pre-existing Array a new
 *   Array will be created based on the shape and element size information
 *   of the model. This is only possible if the model already has a valid
 *   shape and element size. The ownership of the given Array is passed to
 *   the created channel. If the new channel is the only channel in the
 *   model it is activated.
 *   On success this automatically sets the modified flag of the model.
 *
 *   \param channelData A pointer to the integer Array containing the
 *     channel data or the NULL pointer for on-the-fly generation
 *
 *   \return A pointer to the newly created channel or the NULL pointer
 *     if the creation failed
 */
/*======================================================================*/
    // IntegerChannel *addIntegerChannel(atb::Array<int,3> *channelData = NULL);

/*======================================================================*/
/*! 
 *   Create a new RGBChannel object from the given data Array and add it to
 *   this MultiChannelModel. If the Array shape or element size are
 *   incompatible, the function returns a NULL pointer indicating, that
 *   the operation failed. Upon successfull channel addition, all
 *   observers are notified by calling their channelAddEvent() method.
 *   If the NULL pointer is passed instead of a pre-existing Array a new
 *   Array will be created based on the shape and element size information
 *   of the model. This is only possible if the model already has a valid
 *   shape and element size. The ownership of the given Array is passed to
 *   the created channel. If the new channel is the only channel in the
 *   model it is activated.
 *   On success this automatically sets the modified flag of the model.
 *
 *   \param channel A pointer to the vectorial Array containing the
 *     channel data or the NULL pointer for on-the-fly generation
 *
 *   \return A pointer to the newly created channel or the NULL pointer
 *     if the creation failed
 */
/*======================================================================*/
    // RGBChannel *addRGBChannel(
    //     Array<blitz::TinyVector<float,3>,3> *channel = NULL);

/*======================================================================*/
/*! 
 *   Create a new AnnotationChannel object with given Marker::Type and
 *   add it to this MultiChannelModel. Annotation channels can only be
 *   added to models with non-zero shape. If the AnnotationChannel cannot
 *   be created a NULL pointer is returned. Upon successfull channel
 *   addition, all observers are notified by calling their channelAddEvent()
 *   method.
 *   On success this automatically sets the modified flag of the model.
 *
 *   \param markerType The type of Markers the newly created channel will
 *     hold.
 *
 *   \return A pointer to the newly created channel or the NULL pointer
 *     if the creation failed
 */
/*======================================================================*/
    // AnnotationChannel *addAnnotationChannel(
    //     Marker::Type markerType = Marker::Point);

/*======================================================================*/
/*! 
 *   Create a new SplineCurveChannel object from the given ATB::BSpline
 *   pointer and add it to this MultiChannelModel. spline curve channels
 *   can only be added to models with non-zero shape. If the
 *   SplineCurveChannel cannot be created a NULL pointer is returned.
 *   Upon successfull channel addition, all observers are notified by
 *   calling their channelAddEvent() method.
 *   On success this automatically sets the modified flag of the model.
 *
 *   \param spline The spline curve to add to this model
 *
 *   \return A pointer to the newly created channel or the NULL pointer
 *     if the creation failed
 */
/*======================================================================*/
    // SplineCurveChannel *addSplineCurveChannel(
    //     ATB::BSpline< blitz::TinyVector<double,3> > *spline);

/*======================================================================*/
/*! 
 *   Create a new IRoCSChannel object from the given ATB::iRoCS
 *   pointer and add it to this MultiChannelModel. iRoCS channels
 *   can only be added to models with non-zero shape. If the
 *   IRoCSChannel cannot be created a NULL pointer is returned.
 *   Upon successfull channel addition, all observers are notified by
 *   calling their channelAddEvent() method.
 *   On success this automatically sets the modified flag of the model.
 *
 *   \param rct The intrinsic root coordinate transform to add to this
 *     model
 *
 *   \return A pointer to the newly created channel or the NULL pointer
 *     if the creation failed
 */
/*======================================================================*/
    // IRoCSChannel *addIRoCSChannel(ATB::iRoCS *rct);

/*======================================================================*/
/*! 
 *   Add an existing channel (of any kind) to this model. If the
 *   channel cannot be added (i.e. shape and/or element size are different
 *   or the model is empty and you want to add annotations, spline curves
 *   or iRoCS data) -1 is returned. This MultiChannelModel will
 *   be added to the Channel's associated model list. Finally the new channel
 *   is activated. After successfull channel addition, all observers are
 *   notified by calling their channelAddEvent() method. 
 *   On success this automatically sets the modified flag of the model. 
 *
 *   This function is called by all other add...Channel() methods.
 *
 *   \param channel A pointer to the channel to add
 *
 *   \return The index of the added channel in the model or -1 if the channel
 *     could not be added
 */
/*======================================================================*/
    int addChannel(Channel *channel);

/*======================================================================*/
/*! 
 *   Get the index of the given channel in the model.
 *
 *   \param channel A pointer to the channel to find in the model
 *
 *   \return The index of the channel in the model or -1 if the channel
 *     is not contained in the model
 */
/*======================================================================*/
    int find(Channel *channel) const;

/*======================================================================*/
/*! 
 *   Get the index of the first channel with the given name.
 *
 *   \param name The channel name to find in the model
 *
 *   \return The index of the first channel with the given name in the model
 *     or -1 if the name of no channel matched the given string
 */
/*======================================================================*/
    int findChannelByName(std::string const &name) const;

/*======================================================================*/
/*! 
 *   Remove the channel specified by its index from the MultiChannelModel.
 *   Upon successfull channel removal, all observers are notified by
 *   calling their channelRemoveEvent() method (The passed index is already
 *   invalid at that point). The function calls removeModel(this) of the
 *   Channel after successful removal. On success this automatically sets
 *   the modified flag of the model if the model is not empty afterwards.
 *
 *   \param index The index of the Channel to remove from this model
 *
 *   \return A pointer to the Channel for further usage or deletion or
 *     the NULL pointer if the index was out of bounds or the channel
 *     could not be removed because the model would have ended up in an
 *     invalid state
 */
/*======================================================================*/
    Channel *removeChannel(int index);

/*======================================================================*/
/*! 
 *   Get the number of channels of the specified types contained in this
 *   model. If you want to get the number of FloatChannels and
 *   IntegerChannels you would use
 *   \c{nChannels(Channel::Float | Channel::Integer)}
 *
 *   \param types An or'ed set of Channel::Type flags
 *
 *   \return The number of channels of the given types
 */
/*======================================================================*/
    int nChannels(Channel::Types types = Channel::Any) const;
    
/*======================================================================*/
/*! 
 *   Get a read-only immutable pointer to the channel with specified index.
 *
 *   \param index The channel index
 *
 *   \return A pointer onto the Channel at the given index or the NULL
 *     pointer if the index was out of bounds
 */
/*======================================================================*/
    Channel const *channel(int index) const;
    
/*======================================================================*/
/*! 
 *   Get a random access pointer to the channel with specified index.
 *   On success this automatically sets the modified flag of the model.
 *
 *   \param index The channel index
 *
 *   \return A pointer onto the Channel at the given index or the NULL
 *     pointer if the index was out of bounds
 */
/*======================================================================*/
    Channel *channel(int index);
    
/*======================================================================*/
/*! 
 *   Get a read-only iterator to the first channel of this model.
 *
 *   \return A const_iterator to the first channel
 */
/*======================================================================*/
    const_iterator begin() const;

/*======================================================================*/
/*! 
 *   Get a read-only iterator to one element behind the last channel of
 *   this model.
 *
 *   \return A const_iterator beyond the end of the channel vector
 */
/*======================================================================*/
    const_iterator end() const;
    
/*======================================================================*/
/*! 
 *   Get a random-access iterator to the first channel of this model.
 *   For a non-empty model this automatically sets the modified flag of the
 *   model.
 *
 *   \return A const_iterator to the first channel
 */
/*======================================================================*/
    iterator begin();

/*======================================================================*/
/*! 
 *   Get a random-access iterator to one element behind the last channel of
 *   this model.
 *   For a non-empty model this automatically sets the modified flag of
 *   the model.
 *
 *   \return A const_iterator beyond the end of the channel vector
 */
/*======================================================================*/
    iterator end();

/*======================================================================*/
/*! 
 *   Add the given observer to this MultiChannelModel. Observers are any
 *   kind of visualizations that need to be notified on model changes.
 *
 *   \param observer A pointer to the observer to append to the list
 *     of observers for this model
 */
/*======================================================================*/
    void addObserver(MultiChannelModelObserver *observer);

/*======================================================================*/
/*! 
 *   Remove the given observer from this MultiChannelModel.
 *
 *   \param observer A pointer to the observer to remove from this model
 */
/*======================================================================*/
    void removeObserver(MultiChannelModelObserver *observer);

  private:

/*======================================================================*/
/*! 
 *   Checks, whether the given channel can be removed from the model without
 *   breaking it. If the channel is not contained in the model \c false is
 *   returned.
 *
 *   \param channel A pointer to the channel to check for possible removal
 *
 *   \return \c true if the channel can be removed from the model,
 *     \c false otherwise
 */
/*======================================================================*/
    bool _isRemovable(Channel *channel);

/*======================================================================*/
/*! 
 *   Checks, whether the given channel is compatible for addition to this
 *   model and whether it is not already contained.
 *
 *   \param channel A pointer to the channel to check for possible addition
 *
 *   \return \c true if the channel can be added to the model,
 *     \c false otherwise
 */
/*======================================================================*/
    bool _isAddable(Channel *channel);

/*======================================================================*/
/*! 
 *   Create a new unique channel name. The created name will be of the form
 *   channel_<number>, where number is the smallest possible positive
 *   integer to get a not yet assigned name.
 *
 *   \return A new unique channel name
 */
/*======================================================================*/
    std::string _newChannelName() const;

/*======================================================================*/
/*! 
 *   Checks whether all channel names are different. This is important to
 *   be checked before saving the model.
 *
 *   \return \c true if all channel names are unique, \c false otherwise
 */
/*======================================================================*/
    bool _channelNamesUnique() const;

    std::vector<Channel*> _channels;

    std::vector<MultiChannelModelObserver*> _observers;

    int _activeChannel;

    blitz::TinyVector<ptrdiff_t,3> _shapePx;
    blitz::TinyVector<double,3> _elementSizeUm;

    bool _modified;

    friend class Channel;
  };

}

#endif
