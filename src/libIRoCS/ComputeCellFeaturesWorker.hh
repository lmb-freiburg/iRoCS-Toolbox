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

#ifndef COMPUTECELLFEATURESWORKER_HH
#define COMPUTECELLFEATURESWORKER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <libArrayToolbox/Array.hh>
#include <libArrayToolbox/algo/ShellCoordinateTransform.hh>

#include <libProgressReporter/ProgressReporter.hh>

namespace iRoCS
{

/***************************************
 *
 * @param label
 * @param center
 * @param RD  one row of RD, 26 columns
 * @param elSize
 *
 * rd(x) = -1, means this direction radius can not be well measured.
 */
  void extractRD(
      atb::Array<int,3> const &L, int label,
      blitz::TinyVector<double,3> const &center,
      blitz::TinyMatrix<double,3,3> const &axes,
      blitz::TinyVector<double,26> &rd);

/*======================================================================*/
/*!
 *   For each label compute the vector of voxel positions with that label
 *   in L. Voxels with intensities <= 0 or the given backgroundLabel are not
 *   added. The voxel positions for the segment with label i will be
 *   returned at index i-1.
 *
 *   \param L          The segmentation mask Array
 *   \param voxelSets  For each label compute the set of voxels comprising
 *     the segment with that label. The Array must be given with appropriate
 *     size to fit all labels you are interested in.
 *   \param backgroundLabel Do not collect voxel positions for this label
 */
/*======================================================================*/
  void computeVoxelSetsPerLabel(
      atb::Array<int,3> const &L,
      blitz::Array<std::vector< blitz::TinyVector<atb::BlitzIndexT,3> >,1>
      &voxelSets, int backgroundLabel, ProgressReporter *pr = NULL);

/*======================================================================*/
/*!
 *   Compute a convexity measure for a segment based on sample points.
 *
 *   From the given set of segment points, nRandomPairs random pairs are
 *   drawn. If the center point of their connecting line is in the
 *   segment the convexity test returns true and increments the
 *   positive count, otherwise it fails leaving the convexity test count at
 *   its current value. In the end the number of positive test results is
 *   divided by the number of tests to get a value in [0, 1].
 *
 *   \param L The segmentation as integer array containing unique labels
 *     for each segment
 *   \param voxelSet A (sub-)set of voxels stemming from the segment
 *   \param nRandomPairs The number of random point pairs to use for the
 *     convexity test
 *
 *   \return Percentage of positive convexity tests
 */
/*======================================================================*/
  double computeConvexity(
      atb::Array<int,3> const &L,
      std::vector< blitz::TinyVector<atb::BlitzIndexT,3> > const &voxelSet,
      int nRandomPairs = 100000);

/*======================================================================*/
/*!
 *   Compute cell shape features for the given segmentation masks aligned
 *   to the given iRoCS shell coordinate transform.
 *   Features that are computed per segment include: \n
 *     volumes        (1)  - The cell volume in micometers cube \n
 *     normCenters    (3)  - The normalized iRoCS shell coordinate transform
 *                           coordinates. They are normalized so that the outer
 *                           root boundary has a radius value of 1 \n
 *     RD             (26) - The cell shape profile.
 *                           RD(12) + RD(13) = cell extent in axial direction
 *                           RD(10) + RD(15) = cell extent in radial direction
 *                           RD(4) + RD(21) = cell extent in angular
 *                           direction \n
 *     blockSize       (3) - The lengths of the cells' main axes axes \n
 *     volumeOverBlock (1) - The segment - cuboid spanned by blockSize volume
 *                           ratio \n
 *     convexity       (1) - An estimate of the segment's convexity using
 *                           a Montecarlo estimator. \n
 *     neighbor        (n) - The indices of the first n neighboring segments
 *                           If the segment has less than n neighbors, residual
 *                           entries will be -1
 *   Additional information stored per segment: \n
 *     validFlag           - Only if this flag is set, the features are valid
 *     borderFlag          - Is set for segments that touch any volume boundary
 *     centers             - The segment centers in Array coordinates
 *     localAxes           - The main segment axes (rotation matrix)
 *
 *   \param L      The segmentation masks. Voxels with value <=0 are assumed
 *     to be segment boundaries and are ignored
 *   \param sct    The iRoCS shell coordinate transform that will be used
 *     to get the positional features. The shape features will be aligned
 *     to that coordinate system.
 *   \param volumeThresholdUm Segments smaller than this volume threshold
 *     in micrometers cube will be ignored. Their valid flag will be set
 *     to false and the features will be set to all zero.
 *   \param outFileName  The name of the output hdf5 file to store the
 *     per-segment features to.
 *   \param featureGroup The hdf5 base group to store the feature datasets
 *     to.
 *   \param backgroundLabel Give the label of the background segment with
 *     this parameter. If not given (or a negative value is passed) the
 *     background label will be automatically determined as the label
 *     belonging to the largest connected component of the segmentation.
 *   \param connectedComponentLabeling If given the connected components of
 *     the binarized segmentation masks are used instead of the segments as
 *     they are. This is important if the integer value of the segments
 *     indicates class label instead of instance label or if the existing
 *     segment numbering is not consecutive. Labels 0 and <backgroundLabel> are
 *     background for the connected component labeling all other labels
 *     foreground. If given the label array after connected component
 *     labeling will be written to the output group for reference.
 *   \param pr  If given, progress will be reported to this progress reporter
 */
/*======================================================================*/
  void computeCellFeatures(
      atb::Array<int,3> const &L, ShellCoordinateTransform const &sct,
      double volumeThresholdUm, std::string const &outFileName,
      std::string const &featureGroup, int backgroundLabel = -1,
      bool connectedComponentLabeling = false, ProgressReporter *pr = NULL);

}

#endif
