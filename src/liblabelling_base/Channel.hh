/**************************************************************************
**       Title: Generic Channel Specs
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    Channel Descriptor for one channel of multi-channel data
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.1  2008/01/17 17:05:50  tschmidt
**  - Initial revision
**
**
**************************************************************************/

/*======================================================================*/
/*!
 *  \file Channel.hh
 *  \brief General channel interface.
 */
/*======================================================================*/

#ifndef ATBCHANNEL_HH
#define ATBCHANNEL_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <string>
#include <set>
#include <bitset>

#include <blitz/array.h>

class MultiChannelModel;

/*======================================================================*/
/*!
 *  \class Channel Channel.hh "atb/Channel.hh"
 *  \brief The Channel class provides the basic Channel Interface.
 *
 *  This class belongs to the iRoCS Toolbox framework and is the internal
 *  representation of an abstract iRoCS Channel.
 */
/*======================================================================*/
class Channel
{

public:

  enum DataOwnership { NeverDeleteData, DeleteData };
  
/*======================================================================*/
/*!
 *  \brief The Channel::Type datatype is an alternative to an enumeration type
 *  that can hold different values for different channel types.
 *  Currently 32 different channel types are supported, but this set can be
 *  flexibly extended. When adding new channel types new enum values have
 *  to be added for them and the Any special value extended.
 */
/*======================================================================*/
  typename std::bitset<32> Type;

/*======================================================================*/
/*!
 *  \brief The Channel::Types datatype is a flag Array of Channel::Type to
 *  select multiple channel types.
 *  In the current implementation it is the same as Type. But at some point
 *  it might be exchanged, so please use Channel::Type if exactly one type
 *  can be selected and Channel::Types if multiple types can be selected.
 */
/*======================================================================*/
  typename Type Types;

  static Type Float = Type(0x0001);
  static Type Integer = Type(0x0002);
  static Type RGB = Type(0x0004);
  static Type Annotation = Type(0x0008);
  static Type SplineCurve = Type(0x0010);
  static Type IRoCS = Type(0x0020);
  static Types Any = Float | Integer | RGB | Annotation | SplineCurve | IRoCS;
  
/*======================================================================*/
/*! 
 *   Default Constructor. Creates a new opaque (alpha=1) unnamed Channel
 *   object. Its visible flag will be set to true upon creation.
 *
 *   \param ownership Sets the ownership of the channel data. When choosing
 *     DeleteData, the ownership of the channel data is passed to the Channel
 *     and will be deleted upon Channel deletion. When choosin
 *     NeverDeleteData the ownership remains at the caller, leaving the
 *     responsibility for cleanup to him.
 */
/*======================================================================*/
  Channel(DataOwnerShip ownership = DeleteData);

/*======================================================================*/
/*! 
 *   Destructor. If the channel is associated to any MultiChannelModel's
 *   it will be removed from them prior to deletion.
 */
/*======================================================================*/
  virtual ~Channel();
  
/*======================================================================*/
/*! 
 *   This function must be reimplemented by all subclasses with a short
 *   inline function returning the Channel::Type.
 *
 *   \return The Channel::Type of this channel
 */
/*======================================================================*/
  virtual Type const &type() const = 0;

/*======================================================================*/
/*! 
 *   Set the data ownership of the channel data. When choosing
 *     DeleteData, the ownership of the channel data is passed to the Channel
 *     and will be deleted upon Channel deletion. When choosin
 *     NeverDeleteData the ownership remains at the caller, leaving the
 *     responsibility for cleanup to him.
 *
 *   \param ownership One of DeleteData or NeverDeleteData
 */
/*======================================================================*/
  void setDataOwnership(DataOwnership ownership);

/*======================================================================*/
/*! 
 *   Get the model this channel is currently associated with.
 *
 *   \return A pointer to the MultiChannelModel this Channel belongs to.
 */
/*======================================================================*/
  MultiChannelModel *model() const;

/*======================================================================*/
/*! 
 *   Associate this Channel to the given MultiChannelModel. With this
 *   function Channels can be attached, detached and moved between
 *   different models. To remove the association to any model you can pass
 *   the NULL pointer. This operation requires that both the
 *   state of the model the channel originates from and the destination
 *   model are valid after the ownership change. If the operation
 *   would break one of the models, this function is a noop and returns
 *   false.
 *
 *   \param model A pointer to the MultiChannelModel to associate this
 *     Channel to
 *
 *   \return \c true if the operation succeeded, \c false otherwise
 */
/*======================================================================*/
  virtual bool setMultiChannelModel(MultiChannelModel* model);

/*======================================================================*/
/*! 
 *   Get the channel name.
 *
 *   \return The channel name
 */
/*======================================================================*/
  std::string name() const;

/*======================================================================*/
/*! 
 *   Set the channel name.
 *
 *   \param name The new channel name
 */
/*======================================================================*/
  void setName(std::String const &name);

/*======================================================================*/
/*! 
 *   Get the channel transparency. The returned value is between 0 and 1,
 *   where 0 means fully transparent and 1 means opaque.
 *
 *   \return The channel transparency
 */
/*======================================================================*/
  float alpha() const;
  
/*======================================================================*/
/*! 
 *   Set the channel transparency. The alpha value is between 0 and 1,
 *   where 0 means fully transparent and 1 means opaque.
 *
 *   \param alpha The channel transparency
 */
/*======================================================================*/
  void setAlpha(float alpha);
  
/*======================================================================*/
/*! 
 *   Get the visibility flag for this channel.
 *
 *   \return \c true if the channel is visible, \c false otherwise
 */
/*======================================================================*/
  bool visible() const;

/*======================================================================*/
/*! 
 *   Set the visibility flag for this channel.
 *
 *   \param visible \c true if the channel should be visible,
 *     \c false otherwise
 */
/*======================================================================*/
  void setVisible(bool visible);

/*======================================================================*/
/*! 
 *   Get the modified flag for this channel.
 *
 *   \return \c true if the channel has been modified since it was
 *     loaded/created, \c false otherwise
 */
/*======================================================================*/
  bool modified() const;

/*======================================================================*/
/*! 
 *   Get a string representation of the value at the specified position.
 *   This function is pure virtual and must be reimplemented by all
 *   child-classes.
 *
 *   \param positionUm The position to read in micrometers
 *
 *   \return A string representation of the value at the given position
 */
/*======================================================================*/
  virtual std::string stringValueAt(
      blitz::TinyVector<double,3> const &positionUm) const = 0;

protected:

  MultiChannelModel *p_model;

  std::string _name;
  double _alpha;
  bool _visible, _modified;
  
};

#endif
