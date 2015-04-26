/**************************************************************************
 *
 * Copyright (C) 2015 Kun Liu, Thorsten Falk
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

/*
 * lrootShapeAnalysis.h
 *
 *  Created on: Nov 4, 2010
 *      Author: liu
 */

#ifndef LROOTANALYSIS_H_
#define LROOTANALYSIS_H_

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <blitz/array.h>

/*======================================================================*/
/*! 
 *   Compute the number of voxels per intensity in the given integer volume.
 *
 *   \param label    The volume to compute the component voxel count
 *   \param volume   The Array to store the voxel counts to. The number of
 *     voxels for intensity i is returned at index i-1. The volume for
 *     intensity 0 is not computed.
 *   \param maxLabel The maximum label to consider. If -1 is passed the
 *     maximum intensity of the label Array is determined and used as
 *     maximum.
 */
/*======================================================================*/
void volume(
    blitz::Array<int,3> const &label, blitz::Array<double,1> &volume,
    int maxLabel = -1);


/*======================================================================*/
/*! 
 *   Compute the volume in um^3 and center of gravity per intensity in
 *   the given integer volume.
 *
 *   \param label    The volume to compute the component voxel count
 *   \param center   The centers of gravity in um for each intensity level
 *     will be returned in this Array. The center of gravity for intensity i
 *     is returned at index i-1. The center of gravity for intensity 0 is
 *     not computed.
 *   \param volume   The Array to store the volumes in um^3 to. The volume
 *     for intensity i is returned at index i-1. The volume for intensity 0
 *     is not computed.
 *   \param maxLabel The maximum label to consider. If -1 is passed the
 *     maximum intensity of the label Array is determined and used as
 *     maximum.
 *   \param elSize   The voxel extents in micrometers
 */
/*======================================================================*/
void centerAndVolume(
    blitz::Array<int,3> const &label,
    blitz::Array<blitz::TinyVector<double,3>,1> &center,
    blitz::Array<double,1> &volume, int maxLabel,
    blitz::TinyVector<double,3> const &elSize);

/*======================================================================*/
/*! 
 *   Set the marker flag for each voxel, that is part of a segment that is not
 *   marked as too small in the flagTooSmall vector. Internally it is
 *   a parallel implementation of marker = L > 0 && !flagTooSmall(L) where
 *   L in the second part must be understood voxelwise.
 *
 *   \param L      The Array containing the segmentation with segments from 
 *     1 to n. Zero means background and is automatically marked as invalid.
 *   \param marker The output Array containing the valid flag for each voxel
 *     of the segmentation Array.
 *   \param flagTooSmall A vector containing a boolean value of true if the
 *     segment with that index is too small and should be masked as invalid.
 */
/*======================================================================*/
void eraseMarkers(
    blitz::Array<int,3> const &L, blitz::Array<bool,3> &marker,
    blitz::Array<bool,1> const &flagTooSmall);

void labelOnBorder(
    blitz::Array<int,3> const &L, blitz::Array<unsigned char,1> &validFlag);

#endif
