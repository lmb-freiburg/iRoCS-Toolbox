/*
 * lrootShapeAnalysis.h
 *
 *  Created on: Nov 4, 2010
 *      Author: liu
 */

#ifndef LROOTANALYSIS_H_
#define LROOTANALYSIS_H_

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
