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

#ifndef IROCS_FLOATCHANNEL_HH
#define IROCS_FLOATCHANNEL_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "Channel.hh"
#include "FloatColormap.hh"

#include <libArrayToolbox/Array.hh>

namespace iRoCS {

  class FloatChannel : public Channel {

  public:

    FloatChannel();
    ~FloatChannel();

    Type type() const;

    FloatColormap const &colormap() const;
    FloatColormap &colormap();

    /*======================================================================*/
    /*!
     *   Set the edge lengths of a voxel in micrometers.
     *
     *   \param elementSizeUm The new element size in micrometers
     */
     /*======================================================================*/
    void setElementSizeUm(
      blitz::TinyVector<double, 3> const &elementSizeUm);

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
    void setTransformation(
      blitz::TinyMatrix<double, 4, 4> const &transformation);

    /*======================================================================*/
    /*!
     *   Get the lower bound of the world (view) coordinate axis aligned
     *   bounding box enclosing this channel.
     *
     *   \return The bounding box lower bound in micrometer world (view)
     *     coordinates
     */
     /*======================================================================*/
    blitz::TinyVector<double, 3> boundingBoxLowerBoundUm() const;

    /*======================================================================*/
    /*!
     *   Get the upper bound of the world (view) coordinate axis aligned
     *   bounding box enclosing this channel.
     *
     *   \return The bounding box upper bound in micrometer world (view)
     *     coordinates
     */
     /*======================================================================*/
    blitz::TinyVector<double, 3> boundingBoxUpperBoundUm() const;

    /*======================================================================*/
    /*!
     *   Get the value of the underlying atb::Array<float,3> at the given
     *   micrometer position.
     *
     *   \param The position in micrometers to query
     *
     *   \return The value of the atb::Array<float,3> at the given
     *     micrometer position
     */
     /*======================================================================*/
    float valueAt(blitz::TinyVector<double, 3> const &positionUm) const;

    /*======================================================================*/
    /*!
     *   Get a string representation of the value at the given micormeter
     *   position
     *
     *   \return A string representation of the value at the given position
     */
     /*======================================================================*/
    std::string stringValueAt(
      blitz::TinyVector<double, 3> const &positionUm) const;

    atb::Array<float, 3> const &data() const;

    float *dataFirst();

    /*======================================================================*/
    /*!
     *   Load the channel data from the dataset with given name in the given
     *   hdf5 file. Additionally to the dataset, meta-data for the
     *   channel are searched in the /.labelling_qt4 group. If none are found
     *   default values are set.
     *
     *   \param inFile A reference to the hdf5 file to read the channel from
     *   \param dsName The name of the dataset in the hdf5 file containing
     *     the channel data
     *   \param pr If given, progress will be reported to this progress
     *     reporter
     *
     *   \exception BlitzH5Error If for any reason the dataset could not be
     *     read an error of this kind is thrown
     */
     /*======================================================================*/
    void load(
      BlitzH5File const &inFile, std::string const &dsName,
      ProgressReporter *pr = NULL);

    /*======================================================================*/
    /*!
     *   Save this channel to the given hdf5 file with given model index.
     *   Channel meta-data will be written to the group
     *   .labelling_qt4/channel_'index'. The number of channels stored in
     *   .labelling_qt4/nChannels is not altered by this method.
     *
     *   \param outFile a reference to the output hdf5 file opened for writing
     *   \param modelIndex The model index in the .labelling_qt4 meta data group
     *   \param pr If not NULL, progress will be reported via the given
     *     ProgressReporter
     *
     *   \exception BlitzH5Error If the channel cannot be saved for any reason
     */
     /*======================================================================*/
    void save(
      BlitzH5File &outFile, size_t modelIndex, ProgressReporter *pr);

  private:

    /*======================================================================*/
    /*!
     *   This method is called from the general Channel::createFromFile()
     *   factory method or from the load() method above if channel meta-data
     *   where found in the input file.
     */
     /*======================================================================*/
    void load(
      BlitzH5File const &inFile, size_t modelIndex,
      ProgressReporter *pr = NULL);

    /*======================================================================*/
    /*!
     *   This method reads the actual atb::Array.
     */
     /*======================================================================*/
    void loadDataset(
      BlitzH5File const &inFile, std::string const &dsName,
      ProgressReporter *pr = NULL);

    atb::Array<float, 3> _data;

    FloatColormap _colormap;

  };

}

#endif
