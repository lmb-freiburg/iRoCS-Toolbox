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

#ifndef IROCS_CHANNEL_HH
#define IROCS_CHANNEL_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <string>
#include <set>

#include <libBlitzHdf5/BlitzHdf5Light.hh>

#include "Flags.hh"

#include "Observable.hh"

#ifdef _WIN32
#if defined(MultiChannelModel_EXPORTS)
#define MULTICHANNELMODELDLL_API __declspec(dllexport)
#elif defined(MultiChannelModel_USE_DLL)
#define MULTICHANNELMODELDLL_API __declspec(dllimport)
#else
#define MULTICHANNELMODELDLL_API
#endif
#else
#define MULTICHANNNELMODELDLL_API
#endif

namespace iRoCS {

  class MultichannelModel;
  class ChannelObserver;

  /*======================================================================*/
  /*!
   *  \class Channel Channel.hh <liblabelling_qt4/Channel.hh>
   *  \brief The Channel class specifies the interface all channels that
   *    can be added to a MultiChannelModel must fulfil.
   */
   /*======================================================================*/
  class MULTICHANNELMODELDLL_API Channel : public Observable {

#ifdef DEBUG_VERBOSE_XML
    std::string printState() const;
#endif

  public:

    /*======================================================================*/
    /*!
     *  \enum Type Channel.hh <liblabelling_qt4/Channel.hh>
     *  \brief The Type enum contains entries for all available channel types.
     *
     *  When adding new channel types this enum has to be extended, too.
     */
     /*======================================================================*/
    enum Type {
      /** The channel contains 3D intensity data with floating point
       *  precision. The underlying basic class is an atb::Array<float,3> */
      Float = 0x0001,
      /** The channel contains 3D intensity data with integer
       *  precision. The underlying basic class is an atb::Array<int,3> */
      Indexed = 0x0002,
      /** The channel contains 3D RGB color data. Color components are
       *  represented as unsigned 8-Bit values. The underlying basic class
       *  is an atb::Array<blitz::TinyVector<unsigned char,3>,3> */
      RGB = 0x0004,
      /** The channel contains marker based annotations.
       *  The underlying basic class is a std::vector<Marker*> */
      Annotation = 0x0008,
      /** The channel contains a spline curve annotation.
       *  The underlying basic class is
       *  atb::BSpline< blitz::TinyVector<double,3> > */
      SplineCurve = 0x0010,
      /** The channel contains a continuous bent cylinder coordinate system.
       *  The underlying basic class is atb::iRoCS */
      IRoCS = 0x0020,
      /** The channel contains a continuous bent cylinder coordinate
       *  system, that also respects elliptic deformations of the cylinder.
       *  Function to obtain coordinates normalized to the shape of
       *  the tube cross-section are available. The underlying basic
       *  class is a ShellCoordinateTransform */
      IRoCSSCT = 0x0040
    };
    typedef Flags<Type> Types;
    static Types const Any;

    /*======================================================================*/
    /*!
     *   Get a string representation of the given Type.
     *   When adding new channel types they need to be added here, too.
     *
     *   \param type The type to get a string representation for
     *
     *   \return The string representation of the type
     */
     /*======================================================================*/
    static std::string typeToString(Type type);

    /*======================================================================*/
    /*!
     *   Get a Type enum value corresponding to the given string. Only strings
     *   identical to possible outputs of typeToString(Type) will be recognized.
     *   The program will abort if an unknown string is encountered, so never
     *   allow an end-user to provide this value directly!
     *   When adding new channel types they need to be added here, too.
     *
     *   \param typeString The string to get the corresponding enum value for.
     *
     *   \return The enum value corresponding to the given string
     */
     /*======================================================================*/
    static Type typeFromString(std::string const &typeString);

    /*======================================================================*/
    /*!
     *   Create a new Channel object initially without parent model.
     */
     /*======================================================================*/
    Channel();

    /*======================================================================*/
    /*!
     *   Delete this channel and all associated ChannelObserver objects.
     */
     /*======================================================================*/
    virtual ~Channel();

    /*======================================================================*/
    /*!
     *   Get the type of this channel. Concrete implementations must return a
     *   unique value of the Type enum corresponding to the type of the channel.
     *   This is internally used for explicit RTTY, and must be trustworthy!
     *
     *   \return The Type enum value for this kind of channel.
     */
     /*======================================================================*/
    virtual Type type() const = 0;

    /*======================================================================*/
    /*!
     *   Get a reference to the set of ChannelObservers observing this Channel.
     *
     *   \exception A read-only reference to the set of ChannelObservers
     */
     /*======================================================================*/
    std::set<ChannelObserver*> const &observers() const;

    /*======================================================================*/
    /*!
     *   Add this channel to the given MultichannelModel. If the channel was
     *   already associated to a different MultichannelModel when calling this
     *   method the channel is first removed from that model and then added to
     *   this model. You can pass the NULL pointer to remove this Channel from
     *   any model.
     *
     *   \param model The MultichannelModel this channel will be added to
     */
     /*======================================================================*/
    void setModel(MultichannelModel *model);

    /*======================================================================*/
    /*!
     *   Get a pointer to the model this Channel belongs to.
     *
     *   \return A pointer to the parent MultichannelModel of this channel
     *     or NULL if this channel is not associated to a model.
     */
     /*======================================================================*/
    MultichannelModel const *model() const;

    /*======================================================================*/
    /*!
     *   Set the name of this Channel. The name can in principle be
     *   an arbitrary string, but it must conform to an hdf5 path description
     *   if you want to save the model to hdf5. The given string will be
     *   converted to a unique hdf5 path in the model.
     *   Changing the name alters the modified flag of the MultichannelModel,
     *   but it does not alter the modified flag of the channel!
     *
     *   If the name changes, associated observers will be notified via
     *   updateName().
     *
     *   \param name The new channel name
     */
     /*======================================================================*/
    void setName(std::string const &name);

    /*======================================================================*/
    /*!
     *   Get the channel name.
     *
     *   \return The name of the channel
     */
     /*======================================================================*/
    std::string const &name() const;

    /*======================================================================*/
    /*!
     *   Get the components of the name when interpreted as hierarchical
     *   posix-like path. E.g. If the dataset name is /bla/blub, the returned
     *   vector will contain "bla" and "blub". If the current name is the
     *   empty string, an empty vector is returned. Repeated '/' characters will
     *   be treated as if it were only a single slash. It makes no difference
     *   whether the name starts with a slash or not.
     *
     *   \return The individual path components of the given channel name
     */
     /*======================================================================*/
    std::vector<std::string> nameComponents() const;

    /*======================================================================*/
    /*!
     *   Set the filename of this MultichannelModel. The filename should be
     *   set to the original hdf5 filename the channel was read from (it is
     *   automatically set by load(BlitzH5File const &)). If the channel has no
     *   modifications and the output filename equals the filename set through
     *   this function and the file contains structures similar to a channel of
     *   this kind with this name it is assumed that saving is not required. Only
     *   meta-data are updated. You can enforce writing all channel data by
     *   clearing the filename. Changing the filename does not alter any modified
     *   flag.
     *
     *   If the filename changes, associated observers will be notified via
     *   updateFilename().
     *
     *   \param name The new filename for the channel
     */
     /*======================================================================*/
    void setFilename(std::string const &filename);

    /*======================================================================*/
    /*!
     *   Get the (original) hdf5 file name for this channel.
     *
     *   \return The filename this channel was read from.
     */
     /*======================================================================*/
    std::string const &filename() const;

    /*======================================================================*/
    /*!
     *   Set the transparency of this channel. Visualizations should respect
     *   this setting and blend this channel into the model accordingly.
     *   Changing the alpha value alters the modified flag of the
     *   MultichannelModel, but it does not alter the modified flag of the
     *   channel!
     *
     *   If the alpha value changes, associated observers will be notified via
     *   updateAlpha().
     *
     *   \param alpha The new transparency for visualizations of this
     *     channel in [0, 1]
     */
     /*======================================================================*/
    void setAlpha(double alpha);

    /*======================================================================*/
    /*!
     *   Get the transparency of this channel. Visualizations should respect
     *   this setting and blend this channel into the model accordingly.
     *
     *   \return The transparency for visualizations of this channel
     */
     /*======================================================================*/
    double alpha() const;

    /*======================================================================*/
    /*!
     *   Set visibility of this channel. Visualizations should respect
     *   this setting and only render the channel if the visible flag is true.
     *   Changing the visible value alters the modified flag of the
     *   MultichannelModel, but it does not alter the modified flag of the
     *   channel!
     *
     *   If the visible value changes, associated observers will be notified via
     *   updateVisibilityFlag().
     *
     *   \param visible The new visibility flag for visualizations of this channel
     */
     /*======================================================================*/
    void setVisible(bool visible);

    /*======================================================================*/
    /*!
     *   Get visibility of this channel. Visualizations should respect
     *   this setting and only render the channel if the visible flag is true.
     *
     *   \return The visibility flag of this channel
     */
     /*======================================================================*/
    bool visible() const;

    /*======================================================================*/
    /*!
     *   Set the modified flag of this channel. This flag should be set whenever
     *   the model data change. It is mainly used to determine whether the channel
     *   data need to be rewritten when saving it to hdf5. The modified flag of
     *   the parent model is not altered.
     *
     *   If the modified flag value changes, associated observers will be notified
     *   via updateModifiedFlag().
     *
     *   \param modified The new modified flag of this channel
     */
     /*======================================================================*/
    void setModified(bool modified);

    /*======================================================================*/
    /*!
     *   Get the modified flag of this channel.
     *
     *   \return If the channel has unsaved data \c true is returned
     */
     /*======================================================================*/
    bool modified() const;

    /*======================================================================*/
    /*!
     *   Set the edge lengths of a voxel in micrometers. This is only needed
     *   for channels that contain voxel data. For pure vectorial channels it
     *   need not be implemented.
     *
     *   \param elementSizeUm The new element size in micrometers
     */
     /*======================================================================*/
    virtual void setElementSizeUm(
      blitz::TinyVector<double, 3> const &elementSizeUm);

    /*======================================================================*/
    /*!
     *   Get the edge lengths of a voxel in micrometers. This is only needed
     *   for channels that contain voxel data. For pure vectorial channels it
     *   need not be implemented.
     *
     *   \return The element size in micrometers
     */
     /*======================================================================*/
    blitz::TinyVector<double, 3> const &elementSizeUm() const;

    /*======================================================================*/
    /*!
     *   Set the homogoeneous channel transformation matrix. The channel
     *   transformation is a 4x4 homogeneous backwarp transformation matrix
     *   that maps homogeneous world (view) coordinates to homogeneous channel
     *   coordinates. Input and output positions are in micrometers independent
     *   of the resolution of underlying channel data.
     *
     *   \param transformation The new channel transformation matrix
     */
     /*======================================================================*/
    virtual void setTransformation(
      blitz::TinyMatrix<double, 4, 4> const &transformation);

    /*======================================================================*/
    /*!
     *   Get the homogoeneous channel transformation matrix. The channel
     *   transformation is a 4x4 homogeneous backwarp transformation matrix
     *   that maps homogeneous world (view) coordinates to homogeneous channel
     *   coordinates. Input and output positions are in micrometers independent
     *   of the resolution of underlying channel data.
     *
     *   \return The channel transformation matrix
     */
     /*======================================================================*/
    blitz::TinyMatrix<double, 4, 4> const &transformation() const;

    /*======================================================================*/
    /*!
     *   Get the lower bound of the world (view) coordinate axis aligned
     *   bounding box enclosing this channel.
     *
     *   \return The bounding box lower bound in micrometer world (view)
     *     coordinates
     */
     /*======================================================================*/
    virtual blitz::TinyVector<double, 3> boundingBoxLowerBoundUm() const;

    /*======================================================================*/
    /*!
     *   Get the upper bound of the world (view) coordinate axis aligned
     *   bounding box enclosing this channel.
     *
     *   \return The bounding box upper bound in micrometer world (view)
     *     coordinates
     */
     /*======================================================================*/
    virtual blitz::TinyVector<double, 3> boundingBoxUpperBoundUm() const;

    /*======================================================================*/
    /*!
     *   Get a string representation of the value at the given micrometer
     *   position
     *
     *   \return A string representation of the value at the given position
     */
     /*======================================================================*/
    virtual std::string stringValueAt(
      blitz::TinyVector<double, 3> const &positionUm) const = 0;

    /*======================================================================*/
    /*!
     *   Save this channel to the given hdf5 iRoCS Toolbox project file with
     *   given channel index. Channel data is not written if the filename:name
     *   combination points to an existing hdf5 structure that looks like a
     *   channel of this kind and the modified flag is not set. In any case the
     *   entry /.labelling_qt4/channel_<modelIndex> will be updated with the
     *   channel's meta data.
     *
     *   When reimplementing this function, do not forget to call the parent
     *   function (at best before doing anything else)!
     *
     *   \param outFile A reference to an hdf5 file that must be opened for writing
     *   \param modelIndex The channel index within the model
     *   \param pr If not NULL, progress will be reported to this ProgressReporter
     */
     /*======================================================================*/
    virtual void save(
      BlitzH5File &outFile, size_t modelIndex, ProgressReporter *pr = NULL);

    /*======================================================================*/
    /*!
     *   Create a new channel from data stored in an hdf5 iRoCS Toolbox
     *   project file.
     *
     *   \param inFile      A reference to the hdf5 iRoCS project file containing
     *     the .labelling_qt4 meta data group. It must be open for reading.
     *   \param modelIndex  The index of the channel in the iRoCS Toolbox
     *     project file. The file must contain a group
     *     /.labelling_qt4/channel_<modelIndex> containing further channel
     *     meta data and a reference to the dataset or group containing the
     *     channel data.
     *   \param pr          If given, progress will be reported to this progress
     *     reporter
     *
     *   \exception BlitzH5Error If the input file is no iRoCS Toolbox project
     *     file or it does not contain a channel with given index this error
     *     is thrown
     *
     *   \return A pointer to the created Channel object
     */
     /*======================================================================*/
    static Channel *createFromFile(
      BlitzH5File const &inFile, size_t modelIndex,
      ProgressReporter *pr = NULL);

    /*======================================================================*/
    /*!
     *   Create a new channel from data in an hdf5 group or dataset file.
     *   This is a factory method creating a channel of most appropriate type
     *   for the data found in the hdf5 file.
     *
     *   \param InFile  A reference to the hdf5 file opened for reading
     *   \param name    The name of the hdf5 group or dataset containing
     *     the data to create a channel for
     *   \param pr      If given, progress will be reported to this progress
     *     reporter
     *
     *   \exception BlitzH5Error If name cannot be interpreted as any valid
     *     iRoCSToolbox channel data, or for some other reason data could not
     *     be read, this error is thrown
     *
     *   \return A pointer to the created Channel object
     */
     /*======================================================================*/
    static Channel *createFromFile(
      BlitzH5File const &inFile, std::string const &name,
      ProgressReporter *pr = NULL);

    /*======================================================================*/
    /*!
     *   Get the path components of the given channel name.
     *
     *   \param The string to split up into posix-like group and name components
     *
     *   \return The vector containing all parent groups and the object name
     */
     /*======================================================================*/
    static std::vector<std::string> nameComponents(std::string const &name);

    /*======================================================================*/
    /*!
     *   Get a simplified version of the given channel name string with
     *   duplicate and trailing '/' characters removed. If the given string
     *   is empty or only contains slash characters, the empty string is
     *   returned.
     *
     *   \param The string to simplify
     *
     *   \return The simplified string
     */
     /*======================================================================*/
    static std::string simplified(std::string const &name);

  protected:

    /*======================================================================*/
    /*!
     *   Load the Channel from data stored in an hdf5 iRoCS Toolbox
     *   project file. To load a channel of unknown type use the static
     *   factory method
     *   Channel::load(std::string const &, size_t, ProgressReporter)
     *   that creates a new Channel of appropriate type and then calls the
     *   respective implementation of this method.
     *
     *   When reimplementing this function, do not forget to call the parent
     *   function (at best before doing anything else)!
     *
     *   \param inFile      A reference to the input hdf5 file containing
     *     the .labelling_qt4 meta data group.
     *   \param modelIndex  The index of the channel in the iRoCS Toolbox
     *     project file. The file must contain a group
     *     /.labelling_qt4/channel_<modelIndex> containing further channel
     *     meta data and a reference (which may point to other files) to the
     *     dataset or group containing the channel data.
     *   \param pr          If given, progress will be reported to this progress
     *     reporter
     *
     *   \exception BlitzH5Error If the input file is no iRoCS Toolbox project
     *     file or it does not contain a channel with given index this error
     *     is thrown
     */
     /*======================================================================*/
    virtual void load(
      BlitzH5File const &inFile, size_t modelIndex,
      ProgressReporter *pr = NULL);

    /*======================================================================*/
    /*!
     *   Set the bounding box lower bound in micrometers. Reimplementations
     *   should use this function to update the bounding box lower bound before
     *   returning it in boundingBoxLowerBoundUm().
     *
     *   \param boundingBoxLowerBoundUm The new lower bound of the bounding box
     *     in micrometers
     */
     /*======================================================================*/
    void setBoundingBoxLowerBoundUm(
      blitz::TinyVector<double, 3> const &boundingBoxLowerBoundUm);

    /*======================================================================*/
    /*!
     *   Set the bounding box upper bound in micrometers. Reimplementations
     *   should use this function to update the bounding box lower bound before
     *   returning it in boundingBoxUpperBoundUm().
     *
     *   \param boundingBoxUpperBoundUm The new upper bound of the bounding box
     *     in micrometers
     */
     /*======================================================================*/
    void setBoundingBoxUpperBoundUm(
      blitz::TinyVector<double, 3> const &boundingBoxUpperBoundUm);

  protected:

    std::set<ChannelObserver*> _observers;

  private:

    MultichannelModel *p_model;

    std::string _name;
    std::string _filename;
    double _alpha;
    bool _visible;
    bool _modified;

    blitz::TinyVector<double, 3> _elementSizeUm;
    blitz::TinyMatrix<double, 4, 4> _transformation;
    blitz::TinyVector<double, 3> _boundingBoxLowerBoundUm;
    blitz::TinyVector<double, 3> _boundingBoxUpperBoundUm;

    friend class ChannelObserver;

  };

}

IROCS_DECLARE_OPERATORS_FOR_FLAGS(iRoCS::Channel::Types)

#endif
